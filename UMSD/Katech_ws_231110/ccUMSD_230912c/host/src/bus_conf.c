#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bus_conf.h"

#define CONFIG_MAXSIZE_PERLINE 200
#define CONFIG_MAXLINES 200
extern MSD_U16 gFakeDevice;
extern unsigned short g_telnet_port;
MSD_U16 getDeviceValue(char *deviceId);

static char *ltrim(char *str)
{
    if (str == NULL || *str == '\0')
    {
        return str;
    }

    int len = 0;
    char *p = str;
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(str, p, strlen(str) - len + 1);

    return str;
}

static CONF_LIST* parse_file_to_conf_list(char* filename)
{
    char buf[CONFIG_MAXSIZE_PERLINE] = { 0 };
    CONF_LIST *conf_list = NULL;
    CONF_LIST *temp_list = NULL;
    CONF_LIST *temp_list1 = NULL;
    FILE* fp = NULL;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: Cannot open the bus interface configure file: %s\n", filename);
        return NULL;
    }


    while (fgets(buf, CONFIG_MAXSIZE_PERLINE, fp) != NULL)
    {
        char line[CONFIG_MAXSIZE_PERLINE] = { 0 };
        char *token;

        ltrim(buf);
        if ((buf[0] != '#') && (buf[0] != '\0'))
        {
            token = strtok(buf, " \t\n");
            if (strcmp(buf, "}") == 0)
            {
                temp_list = temp_list->parent;
                temp_list1 = NULL;
            }

            while (token != NULL) {
                strcpy(line, token);
                token = strtok(NULL, "\t\n");
                if (token == NULL)
                    break;

                if (strcmp(token, "{") == 0)
                {
                    if (conf_list == NULL)
                    {
                        conf_list = temp_list = (CONF_LIST *)malloc(sizeof(CONF_LIST));
                        memset(temp_list, 0, sizeof(CONF_LIST));
                    }
                    else if (temp_list1 == NULL)
                    {
                        temp_list1 = (CONF_LIST *)malloc(sizeof(CONF_LIST));
                        memset(temp_list1, 0, sizeof(CONF_LIST));
                        temp_list->next = temp_list1;
                        temp_list1->prev = temp_list;
                        temp_list = temp_list1;
                    }
                    temp_list->name = (char*)malloc(strlen(line) + 1);
                    strcpy(temp_list->name, line);
                    temp_list1 = (CONF_LIST *)malloc(sizeof(CONF_LIST));
                    memset(temp_list1, 0, sizeof(CONF_LIST));
                    temp_list->child = temp_list1;
                    temp_list1->parent = temp_list;
                    temp_list = temp_list1;
                }
                else
                {
                    if (temp_list1 == NULL)
                    {
                        temp_list1 = (CONF_LIST *)malloc(sizeof(CONF_LIST));
                        memset(temp_list1, 0, sizeof(CONF_LIST));
                        temp_list->next = temp_list1;
                        temp_list1->prev = temp_list;
                        temp_list1->parent = temp_list->parent;
                        temp_list = temp_list1;
                    }
                    temp_list->name = (char*)malloc(strlen(line) + 1);
                    strcpy(temp_list->name, line);
                    temp_list->value = (char*)malloc(strlen(token) + 1);
                    strcpy(temp_list->value, token);
                    temp_list1 = NULL;
                }
            }
            memset(buf, 0, CONFIG_MAXSIZE_PERLINE);
        }
    }
    fclose(fp);
    return conf_list;
}

static void parse_file_free(CONF_LIST* conf_list)
{
    CONF_LIST *temp_list, *next;

    temp_list = conf_list;
    while (temp_list)
    {
        if (temp_list->child)
            parse_file_free(temp_list->child);

        next = temp_list->next;
        if (temp_list->name)
            free(temp_list->name);
        if (temp_list->value)
            free(temp_list->value);

        temp_list = next;
    }
}


int bus_conf_parse(char* filename, BUS_CONF_STRUCT* bus_conf)
{
    char *tempInterface;
    CONF_LIST* conf_list = NULL;

    conf_list = parse_file_to_conf_list(filename);

    //Pasing the configure file and get the bus config infomation.
    conf_list = conf_list->child;
    if (strcmp(conf_list->value, "SMI_Single") == 0)
    {
        bus_conf->bus_interface = MSD_INTERFACE_SMI;
    }
    else if (strcmp(conf_list->value, "SMI_MultiChip") == 0)
    {
        bus_conf->bus_interface = MSD_INTERFACE_SMI_MULTICHIP;
    }
    else if (strcmp(conf_list->value, "RMU") == 0)
    {
        bus_conf->bus_interface = MSD_INTERFACE_RMU;
    }
	else if (strcmp(conf_list->value, "FW_RMU") == 0)
	{
		bus_conf->bus_interface = MSD_INTERFACE_FW_RMU;
	}
    else if (strcmp(conf_list->value, "Fake") == 0)
    {
        /*Nothing need to do*/
    }
    else
    {
        printf("Incorrect bus interface setting in configure file\n");
        return -1;
    }

    tempInterface = conf_list->value;

    while (conf_list->next)
    {
        CONF_LIST    *temp_sub;
        conf_list = conf_list->next;
        
        if (strcmp(conf_list->name, "telnet_port") == 0)
        {
            g_telnet_port = (unsigned short)strtoul(conf_list->value, NULL, 0);
        }

        if (strcmp(conf_list->name, tempInterface) != 0)
            continue;

        if (strcmp(conf_list->name, "Fake") == 0)
        {
            temp_sub = conf_list->child;
            while (temp_sub)
            {
                if (strcmp(temp_sub->name, "device") == 0)
                {
                    gFakeDevice = getDeviceValue(temp_sub->value);
                }
                temp_sub = temp_sub->next;
            }
            break;
        }

        if (bus_conf->bus_interface == MSD_INTERFACE_SMI)
        {
            temp_sub = conf_list->child;
            while (temp_sub)
            {
                if (strcmp(temp_sub->name, "usb_port") == 0)
                {
                    bus_conf->smi.usb_port = (unsigned int)(strtoul(temp_sub->value, NULL, 0) - 1);
                }
                else if (strcmp(temp_sub->name, "usb_did") == 0)
                {
                    if (strcmp(temp_sub->value, "default") == 0)
                    {
                        bus_conf->smi.defaultUsb = MSD_TRUE;
                    }
                    else
                    {
                        bus_conf->smi.defaultUsb = MSD_FALSE;
                        bus_conf->smi.usb_did = (unsigned int)strtoul(temp_sub->value, NULL, 0);
                    }
                }
                else if (strcmp(temp_sub->name, "smi_protocol") == 0)
                {
                    if (strcmp(temp_sub->value, "XMDIO") == 0)
                    {
                        bus_conf->smi.xmdioFlag = 1;
                    }
                    else
                    {
                        bus_conf->smi.xmdioFlag = 0;
                    }
                }

                temp_sub = temp_sub->next;
            }
        }
        else if (bus_conf->bus_interface == MSD_INTERFACE_SMI_MULTICHIP)
        {
            temp_sub = conf_list->child;
            while (temp_sub)
            {
                if (strcmp(temp_sub->name, "usb_port") == 0)
                    bus_conf->smi.usb_port = (unsigned int)(strtoul(temp_sub->value, NULL, 0) - 1);
                else if (strcmp(temp_sub->name, "usb_did") == 0)
                {
                    if (strcmp(temp_sub->value, "default") == 0)
                    {
                        bus_conf->smi.defaultUsb = MSD_TRUE;
                    }
                    else
                    {
                        bus_conf->smi.defaultUsb = MSD_FALSE;
                        bus_conf->smi.usb_did = (unsigned int)strtoul(temp_sub->value, NULL, 0);
                    }
                }
                else if (strcmp(temp_sub->name, "dev_addr") == 0)
                {
                    bus_conf->dev_num = (MSD_U8)strtoul(temp_sub->value, NULL, 0);
                }
                else if (strcmp(temp_sub->name, "smi_protocol") == 0)
                {
                    if (strcmp(temp_sub->value, "XMDIO") == 0)
                    {
                        bus_conf->smi.xmdioFlag = 1;
                    }
                    else
                    {
                        bus_conf->smi.xmdioFlag = 0;
                    }
                }
                temp_sub = temp_sub->next;
            }
        }
        else if (bus_conf->bus_interface == MSD_INTERFACE_RMU)
        {
            temp_sub = conf_list->child;
            while (temp_sub)
            {
                if (strcmp(temp_sub->name, "dev_num") == 0)
                {
                    bus_conf->dev_num = (MSD_U8)strtoul(temp_sub->value, NULL, 0);
                }
                else if (strcmp(temp_sub->name, "rmu_mode") == 0)
                {
                    if (strcmp(temp_sub->value, "DSA") == 0)
                    {
                        bus_conf->rmu.rmu_mode = MSD_RMU_DSA_MODE;
                    }
                    else if (strcmp(temp_sub->value, "EtherType_DSA") == 0)
                    {
                        bus_conf->rmu.rmu_mode = MSD_RMU_ETHERT_TYPE_DSA_MODE;
                    }
                }
                else if (strcmp(temp_sub->name, "ether_type") == 0)
                {
                    bus_conf->rmu.ethertype = (MSD_U32)strtoul(temp_sub->value, NULL, 0);
                }
                else if (strcmp(temp_sub->name, "nic_name") == 0)
                {
                    strcpy(bus_conf->rmu.nic_name, temp_sub->value);
                }

                temp_sub = temp_sub->next;
            }
        }
		else if (bus_conf->bus_interface == MSD_INTERFACE_FW_RMU)
		{
			temp_sub = conf_list->child;
			while (temp_sub)
			{
				if (strcmp(temp_sub->name, "dev_num") == 0)
				{
					bus_conf->dev_num = (MSD_U8)strtoul(temp_sub->value, NULL, 0);
				}
				else if (strcmp(temp_sub->name, "rmu_mode") == 0)
				{
					if (strcmp(temp_sub->value, "EtherType_DSA") == 0)
					{
						bus_conf->rmu.rmu_mode = MSD_RMU_ETHERT_TYPE_DSA_MODE;
					}
				}
				else if (strcmp(temp_sub->name, "ether_type") == 0)
				{
					bus_conf->rmu.ethertype = (MSD_U32)strtoul(temp_sub->value, NULL, 0);
				}
				else if (strcmp(temp_sub->name, "nic_name") == 0)
				{
					strcpy(bus_conf->rmu.nic_name, temp_sub->value);
				}

				temp_sub = temp_sub->next;
			}
		}
    }
    parse_file_free(conf_list);

    return 0;
}

