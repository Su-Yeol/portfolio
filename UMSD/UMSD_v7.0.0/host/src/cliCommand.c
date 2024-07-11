#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include "msdApi.h"
#include "libcli.h"
#include "apiCLI.h"

unsigned short g_telnet_port = 8888;
#define MAX_LINE_LENGTH             1000

static struct cli_filter_cmds *pfilter_cmd_list = NULL;

static struct cli_def *g_cli;
#ifdef WIN32
HANDLE th_Mutex = NULL;
#endif
void printCallback(const char* str)
{
    cli_bufprint(g_cli, "%s", str);
}

static void strcatbuf(char *buf, char *command, char *argv[], int argc)
{
	int i;
	
	memset(buf, 0, MAX_LINE_LENGTH);
	strcat(buf, command);
	strcat(buf, " ");
	for(i = 0; i < argc; i++)
	{
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}
}
static int cmd_subcmd_help(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	struct cli_command *p;
	struct cli_command *c;
    char temp[50] = { 0 };
    char *token;

    for (p = cli->commands; p; p = p->next)
	{               
		strncpy(temp, cli->commandname, strlen(cli->commandname));
        token = strtok(temp, " ");
		if (token != NULL)
		{
			if (p->children && (strcmp(token, p->command) == 0))
			{
				//cli_show_help(cli, p->children);
				for (c = p->children; c; c = c->next)
				{
					if (c->command && c->callback && cli->privilege >= c->privilege &&
						(c->mode == cli->mode || c->mode == MODE_ANY))
					{
						cli_bufprint(cli, "%s", (c->help != NULL ? c->help : ""));
					}
				}
			}
		}
	}

	return CLI_OK;
}

static void set_filter_cmd(char *completion, char *help)
{
    static int len = 0;

    len++;
    pfilter_cmd_list = (struct  cli_filter_cmds*)realloc(pfilter_cmd_list, len*sizeof(struct  cli_filter_cmds));
    if (pfilter_cmd_list == NULL)
    {
        printf("realloc error for pfilter_cmd_list\n");
		return;
    }
    if ((completion != NULL) && (help != NULL))
    {
        char *temp = (char *)malloc(strlen(completion) + 1);
		if (temp == NULL)
		{
			printf("malloc error for temp\n");
			return;
		}
        strcpy(temp, completion);
        (pfilter_cmd_list + len - 1)->cmd = temp;
        temp = (char *)malloc(strlen(help) + 1);
		if (temp == NULL)
		{
			printf("malloc error for temp\n");
			return;
		}
        strcpy(temp, help);
        (pfilter_cmd_list + len - 1)->help = temp;
    }
    else
    {
        (pfilter_cmd_list + len - 1)->cmd = NULL;
        (pfilter_cmd_list + len - 1)->help = NULL;
    }
}

static cJSON* cjson_sort(cJSON *json)
{  
	
	cJSON *head = (cJSON*)malloc(sizeof(cJSON));
	cJSON *cur = NULL, *p = NULL;
	if (head == NULL) return NULL;
	if ((json == NULL) || (json->next == NULL))
	{
		free(head);
		return json;
	}	
    memset(head, 0, sizeof(cJSON));
    head->next = json;
    json->prev = head;
    p = json;
    cur = json->next;

    while (cur != NULL)
    {
        while (1)
        {
            if ((p == head) || (strcmp(cur->string, p->string) > 0))
            {
                if (p == cur->prev)
                {
                    /*Nothing need to do for sorted node*/
                    p = cur;
                    cur = cur->next;
                }
                else if (cur->next == NULL)
                {
                    /*The last node*/
                    cur->prev->next = NULL;
                    p->next->prev = cur;
                    cur->next = p->next;
                    p->next = cur;
                    cur->prev = p;

                    p = cur;
                    cur = cur->next;
                }
                else
                {
                    cJSON *temp = cur;
                    cur->prev->next = cur->next;
                    cur->next->prev = cur->prev;
                    cur = cur->next;

                    p->next->prev = temp;
                    temp->next = p->next;
                    p->next = temp;
                    temp->prev = p;

                    p = cur->prev;
                }

                break;
            }
            p = p->prev;
        }
        
    }

    json = head->next;
    json->prev = NULL;
    free(head);
    return json;
}

int cliCommand()
{
	struct cli_command *c, *c1;
	struct cli_def *cli;
	int s, x;
	struct sockaddr_in addr;
	int on = 1;
	int i = 0;
    cJSON *root;
    cJSON *temp;
    cJSON *temp1;
    cJSON *temp2;
#ifdef WIN32
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nAddrlen;
	/*use threads for more than one client*/
	HANDLE g_hAcceptThread;
	struct cli_thread cli_th[10];
	int cli_index=0;
#endif

	cli = cli_init();
	cli_set_banner(cli, "UMSD_MCLI test environment");
	cli_set_hostname(cli, "UMSD_MCLI");
	cli_telnet_protocol(cli, 1);

    //Get all the API information
    rootAPIJSON = parseAPIJSONfile();
    temp = rootAPIJSON->child;
    temp1 = temp->child;
    temp1 = cjson_sort(temp1);
    temp->child = temp1;

    root = parseJSONfile();
    if (root == NULL)
    {
        return -1;
    }
    temp = root->child;
    temp1 = cjson_sort(temp);
    root->child = temp1;

    while (temp1 != NULL)
    {
        temp2 = cJSON_GetObjectItem(temp1, "subcmd");        
        if ((temp2 != NULL) && (temp2->child != NULL))
        {
            c = cli_register_command(cli, NULL, temp1->string, cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "desc")->valuestring);
            c1 = cli_register_command(cli, c, "help", cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "help")->valuestring);
            temp = temp2;
            temp2 = temp2->child;
            temp2 = cjson_sort(temp2);
            temp->child = temp2;
            while (temp2 != NULL)
            {
                cmd_register_callback(temp1, temp2);
                cli_register_command(cli, c1, temp2->string, cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, NULL);
                cli_register_command(cli, c, temp2->string, cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp2, "help")->valuestring);
                cJSON *paraList = cJSON_GetObjectItem(temp2, "paraList");
                cJSON *option = cJSON_GetObjectItem(paraList, "[options](default value for option item is 0):");
                if (option != NULL)
                {
                    cJSON *filterList = option->child;
                    while (filterList != NULL)
                    {
                        char arr[100] = { 0 };
                        strncpy(arr, filterList->string,strlen(filterList->string));
                        if (arr[0] == '-')
                        {
                            memset(arr, 0, 100);
							strncat(arr, temp1->string, strlen(temp1->string));
                            strncat(arr, " ",1);
							strncat(arr, temp2->string, strlen(temp2->string));

                            set_filter_cmd(filterList->string, arr);                      
                        }
                        filterList = filterList->next;
                    }
                }
                temp2 = temp2->next;
            }
            temp2 = cJSON_GetObjectItem(temp1, "subcmd");
            CALLBACE tempFunc = paraHelp;
            cJSON_AddItemToObject(temp2, "help", cJSON_CreateNumber((intptr_t)tempFunc));
        }
        else
        {
            if (cmd_direct_register_callback(temp1))
                cli_register_command(cli, NULL, temp1->string, cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "desc")->valuestring);
        }
        temp1 = temp1->next;
    }
    set_filter_cmd(NULL, NULL);
    cli_set_filter(pfilter_cmd_list);
    
    apiCLI_alloc_buf();
    g_cli = cli;
    setPrintCallback(printCallback);
    msdSetStringPrintCallback(printCallback);

#ifdef WIN32
    /*init the WSA*/
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        printf("init the WSA error\n");
        return 0;
    }

    /*Create the socket*/
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s == INVALID_SOCKET)
    {
        printf("Create socket error\n");
        return -1;
    }

    /*bind the IP and port*/
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_telnet_port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if(bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        printf("bind error\n");
		closesocket(s);
        return -1;
    }

    /*Listen*/
    if(listen(s, 1) == SOCKET_ERROR)
    {
        printf("listen error \n");
		closesocket(s);
        return -1;
    }
    printf("Listening on port %d for telnet\n", g_telnet_port);

	th_Mutex = CreateMutex(NULL, FALSE, NULL);
    nAddrlen = sizeof(addr);
	while (x = accept(s, (SOCKADDR *)&addr, &nAddrlen))
    {
       
        if(x == INVALID_SOCKET)
        {
			_pclose(x);
            printf("accept error \n");
            continue;
        }
        printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));
		cli_th[cli_index].cli = malloc(sizeof(struct cli_def));
		if (cli_th[cli_index].cli == NULL)
		{
			_pclose(x);
			printf("accept socket malloc error \n");
			continue;
		}
		memcpy(cli_th[cli_index].cli, cli, sizeof(struct cli_def));
		//cli_th[cli_index].cli = cli;
		cli_th[cli_index].sock = x;
		g_hAcceptThread = CreateThread(NULL, 0, AcceptThread, &cli_th[cli_index], 0, 0);
		cli_index++;
		//WaitForSingleObject(g_hAcceptThread, INFINITE);
		CloseHandle(g_hAcceptThread);
		
    }

#else
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return 1;
	}
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(g_telnet_port);
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
        close(s);
		perror("bind");
		return 1;
	}

	if (listen(s, 1) < 0)
	{
        close(s);
		perror("listen");
		return 1;
	}

	printf("Listening on port %d for telnet\n", g_telnet_port);

	while ((x = accept(s, NULL, 0)))
	{
		int pid = fork();
		if (pid < 0)
		{
            close(x);
			perror("fork");
			return 1;
		}

		/* parent */
		if (pid > 0)
		{
			socklen_t len = sizeof(addr);
			if (getpeername(x, (struct sockaddr *) &addr, &len) >= 0)
				printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

			close(x);
			continue;
		}

		/* child */
		close(s);
		cli_loop(cli, x);
		exit(0);
	}
#endif

	apiCLI_free_buf();
	cli_done(cli);
	return 0;
}
#ifdef WIN32
DWORD WINAPI AcceptThread(LPVOID lpParameter)
{
	/*printf("accept thread open\n");*/
	struct cli_thread *cli_th = (struct cli_thread*)lpParameter;
	cli_loop(cli_th->cli,cli_th->sock);
	shutdown(cli_th->sock, SD_BOTH);
	_pclose(cli_th->sock);
	return 0;
}
#endif