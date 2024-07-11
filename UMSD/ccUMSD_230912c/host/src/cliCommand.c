#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //sy.kim, fork()
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
#define MAX_LINE_LENGTH 1000

static struct cli_filter_cmds *pfilter_cmd_list = NULL;

static struct cli_def *g_cli;
#ifdef WIN32
HANDLE th_Mutex = NULL;
#endif

#if 1 // jay.choi.230901.get.API
extern MSD_U32 linkState;
#endif

// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ SY.Kim ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
// char *fir_vlan_delentry(uint8_t vid)
// {
// 	char *msgBuf;
// 	char *rc;

// 	msgBuf = malloc(4096);
// 	if (NULL == msgBuf)
// 	{
// 		printf("malloc failed\n");
// 		return NULL;
// 	}
// 	rc = malloc(4096);
// 	if (NULL == rc)
// 	{
// 		printf("rc malloc failed\n");
// 		return NULL;
// 	}

// 	memset(msgBuf, 0, 4096);
// 	memset(rc, 0, 4096);
// 	// vlan delEntry 1
// 	sprintf(msgBuf, "%s %s %d", "vlan", "delEntry", vid);
// 	sprintf(msgBuf + strlen(msgBuf), "\r\n");

// 	rc = strdup(msgBuf);

// 	if (msgBuf)
// 	{
// 		free(msgBuf);
// 	}

// 	return rc;
// }
// char *fir_vlan_addentry(uint8_t fid, uint8_t vid, uint8_t portTag[])
// {
// 	char *msgBuf;
// 	char *rc;

// 	msgBuf = malloc(4096);
// 	if (NULL == msgBuf)
// 	{
// 		printf("msgBufmalloc failed\n");
// 		return NULL;
// 	}
// 	rc = malloc(4096);
// 	if (NULL == rc)
// 	{
// 		printf("rc malloc failed\n");
// 		return NULL;
// 	}

// 	memset(msgBuf, 0, 4096);
// 	memset(rc, 0, 4096);
// 	// vlan addEntry -fid fid -vid vid -TagP portTag[]
// 	sprintf(msgBuf, "%s %s %s 0x%x %s 0x%x %s %s%d %d %d %d %d %d %d %d %d %d%s",
// 			"vlan", "addEntry",
// 			"-fid", fid,
// 			"-vid", vid,
// 			"-TagP", "{",
// 			portTag[0], portTag[1], portTag[2], portTag[3], portTag[4], portTag[5], portTag[6], portTag[7], portTag[8], portTag[9],
// 			"}");
// 	sprintf(msgBuf + strlen(msgBuf), "\r\n");

// 	rc = strdup(msgBuf);

// 	if (msgBuf)
// 	{
// 		free(msgBuf);
// 	}

// 	return rc;
// }
// // char *fir_connect(int g_port)
// char *fir_connect(char *msgbuf, int g_port)
// {
// 	struct sockaddr_in addr_server;
// 	socklen_t addr_len_server;
// 	char *sendmsgbuf = msgbuf;
// 	char *readmsgbuf;
// 	int sedbytes, recvbytes;

// 	int fir_sock;
// 	fir_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
// 	if (fir_sock < 0)
// 	{
// 		perror("fir socket error\n");
// 		return NULL;
// 	}

// 	memset(&addr_server, 0, sizeof(addr_server));
// 	addr_server.sin_family = AF_INET;
// 	addr_server.sin_port = htons(g_port);
// 	inet_aton("127.0.0.1", (struct in_addr *)&addr_server.sin_addr.s_addr);

// 	// 02.19
// 	while (1)
// 	{
// 		sendmsgbuf = (char *)malloc(4096);
// 		if (sendmsgbuf == NULL)
// 		{
// 			perror("readmsgbuf malloc error");
// 			free(sendmsgbuf);
// 			return NULL;
// 		}

// 		readmsgbuf = (char *)malloc(4096);
// 		if (readmsgbuf == NULL)
// 		{
// 			perror("readmsgbuf malloc error");
// 			free(readmsgbuf);
// 			return NULL;
// 		}

// 		addr_len_server = sizeof(addr_server);
// 		// strcpy(sendmsgbuf, fir_vlan_delentry(0x1));
// 		// sendmsgbuf = fir_vlan_delentry(1);
// 		printf("fir connect sendmsgbuf: %s\n", sendmsgbuf);

// 		sedbytes = sendto(fir_sock, sendmsgbuf, strlen(sendmsgbuf), 0, (struct sockaddr *)&addr_server, addr_len_server);
// 		printf("fir connect sendto success\n");
// 		if (sedbytes < 0)
// 		{
// 			perror("sendmsgbuf sendto error\n");
// 			free(sendmsgbuf);
// 			return NULL;
// 		}

// 		recvbytes = recvfrom(fir_sock, readmsgbuf, 4096, 0, (struct sockaddr *)&addr_server, &addr_len_server);
// 		printf("fir connect recv success\n");
// 		if (recvbytes < 0)
// 		{
// 			perror("readmsgbuf recvfrom error\n");
// 			free(readmsgbuf);
// 			free(sendmsgbuf);
// 			return NULL;
// 		}
// 		printf("sedbytes %d||recvbytes %d\n", sedbytes, recvbytes);
// 		// Free allocated memory
// 		// free(sendmsgbuf);
// 		// free(readmsgbuf);
// 	}
// 	close(fir_sock);

// 	return sendmsgbuf;
// }
// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ SY.Kim ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ //
void printCallback(const char *str)
{
#if 1 // jay.choi.230901.get.API.debugging
	printf("printCallback : %s", str);
#endif
	cli_bufprint(g_cli, "%s", str);
}

static void strcatbuf(char *buf, char *command, char *argv[], int argc)
{
	int i;

	memset(buf, 0, MAX_LINE_LENGTH);
	strcat(buf, command);
	strcat(buf, " ");
	for (i = 0; i < argc; i++)
	{
		strcat(buf, argv[i]);
		strcat(buf, " ");
	}
}
static int cmd_subcmd_help(struct cli_def *cli, const char *command, char *argv[], int argc)
{
	struct cli_command *p;
	struct cli_command *c;
	char temp[50] = {0};
	char *token;

	for (p = cli->commands; p; p = p->next)
	{
		strncpy(temp, cli->commandname, strlen(cli->commandname));
		token = strtok(temp, " ");
		if (token != NULL)
		{
			if (p->children && (strcmp(token, p->command) == 0))
			{
				// cli_show_help(cli, p->children);
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
	pfilter_cmd_list = (struct cli_filter_cmds *)realloc(pfilter_cmd_list, len * sizeof(struct cli_filter_cmds));
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

static cJSON *cjson_sort(cJSON *json)
{

	cJSON *head = (cJSON *)malloc(sizeof(cJSON));
	cJSON *cur = NULL, *p = NULL;
	if (head == NULL)
		return NULL;
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

#if 1 // jay.choi.local.config
int cliCommand(int lconfig)
#else
int cliCommand()
#endif
{
	struct cli_command *c, *c1;
	struct cli_def *cli;
	int s, x;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
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
	int cli_index = 0;
#endif

	cli = cli_init();
	cli_set_banner(cli, "UMSD_MCLI test environment");
	cli_set_hostname(cli, "UMSD_MCLI");
	cli_telnet_protocol(cli, 1);

	// Get all the API information
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
	// API command register
	while (temp1 != NULL)
	{
		temp2 = cJSON_GetObjectItem(temp1, "subcmd");
		if ((temp2 != NULL) && (temp2->child != NULL))
		{
#ifdef J_DEBUG
			printf("temp2=%s, temp2->child=%s\n", temp2->string, temp2->child->string);
#endif
			// if a subcommand exists
			c = cli_register_command(cli, NULL, temp1->string, cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "desc")->valuestring);
			c1 = cli_register_command(cli, c, "help", cmd_subcmd_help, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "help")->valuestring);
			temp = temp2;
			temp2 = temp2->child;
			temp2 = cjson_sort(temp2);
			temp->child = temp2;
			// subcommand register
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
						char arr[100] = {0};
						strncpy(arr, filterList->string, strlen(filterList->string));
						if (arr[0] == '-')
						{
							memset(arr, 0, 100);
							strncat(arr, temp1->string, strlen(temp1->string));
							strncat(arr, " ", 1);
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

#ifdef J_DEBUG
			printf("temp1=%s\n", temp1->string);
#endif
			// if a subcommand not exists
			if (cmd_direct_register_callback(temp1))
				cli_register_command(cli, NULL, temp1->string, cmd_operation, PRIVILEGE_UNPRIVILEGED, MODE_ANY, cJSON_GetObjectItem(temp1, "desc")->valuestring);
		}
		temp1 = temp1->next;
	}
	// filter setting
	set_filter_cmd(NULL, NULL);
	cli_set_filter(pfilter_cmd_list);
	// CLI buffer allocation and setup
	apiCLI_alloc_buf();
	g_cli = cli;
#if 1 // jay.choi.230901.get.API // jay.choi.local.config.fix.segmentation.fault.http://clm.lge.com/issue/browse/SDVTSN-8
	if (!lconfig)
#endif
		setPrintCallback(printCallback);
	msdSetStringPrintCallback(printCallback);

// #if 1 // jay.choi.local.config
#if 1 // jay.choi.230901.get.API.https://yeni03-0w0.tistory.com/23
	if (lconfig)
	{
		socklen_t addr_len;
		int sock_fd;
		socklen_t client_addr_len;
		int sock_fd_client;

		char *msgbuf;
		int rc;
		int bytes = 0;

		sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock_fd < 0)
		{
			perror("socket");
			return 1;
		}
		// setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		memset(&client_addr, 0, sizeof(client_addr));
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // htonl(INADDR_ANY); sy.kim
		addr.sin_port = htons(g_telnet_port);
		addr_len = sizeof(addr);

		if (bind(sock_fd, (struct sockaddr *)&addr, addr_len) < 0)
		{
			perror("bind error");
			return 1;
		}
		printf("Server IP: %s, Port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		
		while (1)
		{
			client_addr_len = sizeof(client_addr);

			msgbuf = (char *)malloc(CLI_MAX_LINE_LENGTH);
			if (NULL == msgbuf)
			{
				perror("msgbuf malloc error");
				return 1;
			}

			bytes = recvfrom(sock_fd, msgbuf, CLI_MAX_LINE_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);
			if (bytes < 0)
			{
				perror("cliCommand recvfrom");
				return 1;
			}
			else
			{
				printf("%zd bytes recv sucess\n", bytes);
			}

			printf("recvmsg(%s) : bytes=%d\n", msgbuf, bytes);
#ifdef J_DEBUG
			rc = cli_run_command(cli, msgbuf);
			printf("cli_run_command() : rc=%d\n", rc);
			if (rc == CLI_QUIT)
#else
			if (cli_run_command(cli, msgbuf) == CLI_QUIT)
#endif
			{
				perror("cli_run_command");
				return 1;
			}

#if 1 // jay.choi.230901.get.API
			snprintf(msgbuf, CLI_MAX_LINE_LENGTH, "%d", linkState);
			sendto(sock_fd, msgbuf, bytes, 0, (struct sockaddr *)&client_addr, client_addr_len);
			free(msgbuf);
#endif
		}

		apiCLI_free_buf();
		cli_done(cli);
		return 0;
	}
#else
	if (lconfig)
	{

		//		struct sockaddr_in addr;
		socklen_t addr_len;
		int sock_fd = -1;
		int rc;

		fd_set fds, sel_fds;
		int max_fd;

		char *msgbuf;
		struct sockaddr_in client_addr;
		struct msghdr msg;
		struct iovec iov;
		int bytes = 0;

		sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock_fd < 0)
		{
			perror("socket");
			return 1;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(g_telnet_port);
		inet_aton("127.0.0.1", (struct in_addr *)&addr.sin_addr.s_addr);
		addr_len = sizeof(addr);

		rc = bind(sock_fd, (struct sockaddr *)&addr, addr_len);
		if (rc < 0)
		{
			close(sock_fd);
			perror("bind");
			return 1;
		}

		FD_ZERO(&fds);
		FD_SET(sock_fd, &fds);

		max_fd = sock_fd;

		do
		{
			sel_fds = fds;
			rc = select(max_fd + 1, &sel_fds, NULL, NULL, NULL);
			if (rc == -1)
			{
				perror("select");
				return 1;
			}
			else
			{
				if (FD_ISSET(sock_fd, &sel_fds))
				{
					msgbuf = (char *)malloc(CLI_MAX_LINE_LENGTH);
					if (NULL == msgbuf)
					{
						perror("malloc");
						return 1;
					}
					memset(&msg, 0, sizeof(msg));
					memset(&client_addr, 0, sizeof(client_addr));
					memset(msgbuf, 0, CLI_MAX_LINE_LENGTH);

					iov.iov_len = CLI_MAX_LINE_LENGTH;
					iov.iov_base = msgbuf;
					msg.msg_name = &client_addr;
					msg.msg_namelen = sizeof(client_addr);
					msg.msg_iov = &iov;
					msg.msg_iovlen = 1;

					bytes = recvmsg(sock_fd, &msg, 0);

					printf("recvmsg(%s) : bytes=%d\n", msgbuf, bytes);

					if (bytes <= 0)
					{
						perror("recvmsg");
						free(msgbuf);
						return 1;
					}

#ifdef J_DEBUG
					rc = cli_run_command(cli, msgbuf);
					printf("cli_run_command() : rc=%d\n", rc);
					if (rc == CLI_QUIT)
#else
					if (cli_run_command(cli, msgbuf) == CLI_QUIT)
#endif
					{
						perror("cli_run_command");
						return 1;
					}

#if 1 // jay.choi.230901.get.API
	  // send the data to client here!
#endif
				}
			}
		} while (1);

		apiCLI_free_buf();
		cli_done(cli);
		return 0;
	}
#endif
	// #endif

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
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
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
			perror("cliCommand fork error");
			return 1;
		}

		/* parent */
		if (pid > 0)
		{
			socklen_t len = sizeof(addr);
			if (getpeername(x, (struct sockaddr *)&addr, &len) >= 0) // connect > only TCP
				printf(" * accepted connection from %s\n", inet_ntoa(addr.sin_addr));

			close(x); // parent close
			continue;
		}

		/* child */
		close(s);
		cli_loop(cli, x); // default
		exit(0);
	}
	// #endif
	apiCLI_free_buf();
	cli_done(cli);
	return 0;
}
#ifdef WIN32
DWORD WINAPI AcceptThread(LPVOID lpParameter)
{
	/*printf("accept thread open\n");*/
	struct cli_thread *cli_th = (struct cli_thread *)lpParameter;
	cli_loop(cli_th->cli, cli_th->sock);
	shutdown(cli_th->sock, SD_BOTH);
	_pclose(cli_th->sock);
	return 0;
}
#endif
