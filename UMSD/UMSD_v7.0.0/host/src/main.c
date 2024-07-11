#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Wrapper.h"
#include "msdApi.h"
#include "rmuPcap.h"
#include "bus_conf.h"
#ifdef WIN32
#include <Winsock2.h>  
#include <Iptypes.h>  
#include <IPHlpApi.h>
#else
#include   <sys/ioctl.h> 
#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <net/if.h> 
#endif

/****************************************************************************/
/* Parameter functions.                                                     */
/****************************************************************************/
extern MSD_QD_DEV       *qddev;
extern char    pcap_device_name[512];
extern char    AdapterList[Max_Num_Adapter][512];
extern int     NumOfNIC;
extern unsigned char  AdapterMACAddr[6];
extern MSD_RMU_MODE gRmuMode;
extern MSD_U8 gFWRmuFlag;
extern MSD_U32 gEtherTypeValue;
extern MSD_U8 gXMDIOFlag;
extern MSD_U8 sohoDevNum;
extern MSD_U16 gFakeDevice;
extern unsigned short g_telnet_port;


/****************************************************************************/
/* Function Declaration                                                     */
/****************************************************************************/
MSD_STATUS qdInit(int baseAddr, int bus_interface, MSD_U16 tempDeviceId);
void qdClose();
void clean_stdin(void);
void apiCLI(void);
int cliCommand();
int strIcmp(const char* p1, const char* p2);

char* SWGetDeviceName(int devId)
{
	char* ret;
	switch (devId)
	{
	case MSD_88E6190:
	case MSD_88E6190X:
	case MSD_88E6390:
	case MSD_88E6390X:
	case MSD_88E6290:
		ret = "Peridot";
		break;

	case MSD_88E6141:
	case MSD_88E6341:
		ret = "Topaz";
		break;
	case MSD_88E6352:
	case MSD_88E6172:
	case MSD_88E6176:
	case MSD_88E6240:
		ret = "Agate";
		break;
	case MSD_88E6320:
	case MSD_88E6321:
		ret = "Pearl";
		break;
	case MSD_88E6191X:
	case MSD_88E6393X:
	case MSD_88E6193X:
	case MSD_88E6391X:
		ret = "Amethyst";
		break;
	case MSD_88Q5072:
		ret = "Oak";
		break;
	case MSD_88Q6113:
		ret = "Spruce";
		break;
	case MSD_88Q5040Z1:
		ret = "Bonsai-Z1";
		break;
	case MSD_88Q5151:
	case MSD_88Q5152:
	case MSD_88Q5192:
		ret = "Fir";
		break;
	case MSD_88Q5040:
		ret = "Bonsai";
		break;
	default:
		ret = "Unknown";
		break;
	}
	return ret;
}
MSD_U16 getDeviceValue(char *deviceId)
{
    MSD_U16 value = 0;

    if (strIcmp(deviceId, "88E6141") == 0)
        value = 0x3400;
    else if (strIcmp(deviceId, "88E6341") == 0)
        value = 0x3410;
    else if (strIcmp(deviceId, "88E6390X") == 0)
        value = 0x0A10;
    else if (strIcmp(deviceId, "88E6390") == 0)
        value = 0x3900;
    else if (strIcmp(deviceId, "88E6190X") == 0)
        value = 0x0A00;
    else if (strIcmp(deviceId, "88E6190") == 0)
        value = 0x1900;
    else if (strIcmp(deviceId, "88E6290") == 0)
        value = 0x2900;
    else if (strIcmp(deviceId, "88E6352") == 0)
        value = 0x3520;
    else if (strIcmp(deviceId, "88E6172") == 0)
        value = 0x1720;
    else if (strIcmp(deviceId, "88E6176") == 0)
        value = 0x1760;
    else if (strIcmp(deviceId, "88E6240") == 0)
        value = 0x2400;
    else if (strIcmp(deviceId, "88E6320") == 0)
        value = 0x1150;
    else if (strIcmp(deviceId, "88E6321") == 0)
        value = 0x3100;
    else if (strIcmp(deviceId, "88E6191X") == 0)
        value = 0x1920;
    else if (strIcmp(deviceId, "88E6393X") == 0)
        value = 0x3930;
    else if (strIcmp(deviceId, "88E6193X") == 0)
        value = 0x1930;
    else if (strIcmp(deviceId, "88E6391X") == 0)
        value = 0x3910;
	else if (strIcmp(deviceId, "88Q5072") == 0)
		value = 0xA721;
	else if (strIcmp(deviceId, "88Q6113") == 0)
		value = 0xF131;
	else if (strIcmp(deviceId, "88Q5040Z1") == 0)
		value = 0xA402;
	else if (strIcmp(deviceId, "88Q5151") == 0)
		value = 0xb510;
	else if (strIcmp(deviceId, "88Q5152") == 0)
		value = 0xb520;
	else if (strIcmp(deviceId, "88Q5192") == 0)
		value = 0xb920;
	else if (strIcmp(deviceId, "88Q5040") == 0)
		value = 0x1240;
    else
        value = 1;

    return value;
}
static int openBusWithCfgFile(char *filename, int *bus_interface, MSD_U32 *baseAddr)
{
    int index;
    BUS_CONF_STRUCT bus_conf;

    memset(&bus_conf, 0, sizeof(BUS_CONF_STRUCT));
	bus_conf_parse(filename, &bus_conf);
    
    if (gFakeDevice != 0)
        return 0;

    sohoDevNum = bus_conf.dev_num;
    *bus_interface = (int)(bus_conf.bus_interface);
    *baseAddr = bus_conf.dev_num;
    gRmuMode = bus_conf.rmu.rmu_mode;
    gEtherTypeValue = bus_conf.rmu.ethertype;
    gXMDIOFlag = bus_conf.smi.xmdioFlag;

    switch (bus_conf.bus_interface)
    {
    case MSD_INTERFACE_SMI:
    case MSD_INTERFACE_SMI_MULTICHIP:
    {
        int ret = 0;
        char buf[64] = { 0 };
        setUSBPort(bus_conf.smi.usb_port);
        if (bus_conf.smi.defaultUsb == MSD_TRUE)
        {
            ret = openDefaultUSBDriver();
            if (ret != 0)
            {
                printf("Init USB Connect error, error code:%d\n", ret);
                return -1;
            }
        }
        else
        {
            sprintf(buf, "0x%04x:Hi-Speed", bus_conf.smi.usb_did);
            ret = openUSBDriver(buf, bus_conf.smi.usb_port);
            if (ret != 0)
            {
                printf("Init USB Connect error, error code:%d\n", ret);
                return -1;
            }
        }
    }
    break;
    case MSD_INTERFACE_RMU:
        pcap_rmuGetDeviceList();
        {
#ifdef WIN32
            IP_ADAPTER_INFO AdapterInfo[16];
            DWORD dwBuflen = sizeof(AdapterInfo);

            DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBuflen);
            PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

            do{
                if (strstr(bus_conf.rmu.nic_name, pAdapterInfo->Description) != 0)
                {
                    memcpy(AdapterMACAddr, pAdapterInfo->Address, 6);
                    for (index = 0; index < Max_Num_Adapter; index++)
                    {
                        if (strstr(AdapterList[index], pAdapterInfo->AdapterName) != 0)
                        {
                            strcpy(pcap_device_name, AdapterList[index]);
                            break;
                        }
                    }
                    break;
                }
                pAdapterInfo = pAdapterInfo->Next;
            } while (pAdapterInfo);
#else
            struct   ifreq   ifreq;
            int   sock;

            strcpy(pcap_device_name, bus_conf.rmu.nic_name);
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0)
            {
                perror("socket ");
                return   2;
            }
            strncpy(ifreq.ifr_name, pcap_device_name, IF_NAMESIZE-1);
            if (ioctl(sock, SIOCGIFHWADDR, &ifreq) <0)
            {
                close(sock);
                perror("ioctl ");
                return   3;
            }
            memcpy(AdapterMACAddr, ifreq.ifr_hwaddr.sa_data, 6);
#endif
            if (pcap_rmuOpenEthDevice() != 0)
            {
                printf("NIC Open Failed\n");
                return -1;
            }
        }
        break;
	case MSD_INTERFACE_FW_RMU:
		gFWRmuFlag = 1;
		pcap_rmuGetDeviceList();
		{
#ifdef WIN32
			IP_ADAPTER_INFO AdapterInfo[16];
			DWORD dwBuflen = sizeof(AdapterInfo);

			DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBuflen);
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

			do{
				if (strstr(bus_conf.rmu.nic_name, pAdapterInfo->Description) != 0)
				{
					memcpy(AdapterMACAddr, pAdapterInfo->Address, 6);
					for (index = 0; index < Max_Num_Adapter; index++)
					{
						if (strstr(AdapterList[index], pAdapterInfo->AdapterName) != 0)
						{
							strcpy(pcap_device_name, AdapterList[index]);
							break;
						}
					}
					break;
				}
				pAdapterInfo = pAdapterInfo->Next;
			} while (pAdapterInfo);
#else
			struct   ifreq   ifreq;
			int   sock;

			strcpy(pcap_device_name, bus_conf.rmu.nic_name);
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0)
			{
				perror("socket ");
				return   2;
			}
			strncpy(ifreq.ifr_name, pcap_device_name, IF_NAMESIZE - 1);
			if (ioctl(sock, SIOCGIFHWADDR, &ifreq) <0)
			{
				close(sock);
				perror("ioctl ");
				return   3;
			}
			memcpy(AdapterMACAddr, ifreq.ifr_hwaddr.sa_data, 6);
#endif
			if (pcap_rmuOpenEthDevice() != 0)
			{
				printf("NIC Open Failed\n");
				return -1;
			}
		}
		break;
    default:
        break;
    }

    return 0;
}
static int openBusInterface(int *bus_interface, MSD_U32 *baseAddr)
{
    int command = 0;

    do
    {
        printf("Interface List:\n");
        printf("\t0 : SMI Interface\n");
        printf("\t1 : SMI Multi-Chip\n");
        printf("\t2 : RMU Interface\n");
		printf("\t3 : FW RMU Interface\n");
        printf("\t4 : Fake Device\n");
        printf("Please select one interface: ");
        if(scanf(" %c", (char*)(&command)) == 0)
			printf("Failed to read the input interface index\n");
        clean_stdin();
        switch (command)
        {
        case '1':
            printf("please enter the base address(hex)\n");
            if(scanf("%x", (int*)baseAddr) == 0)
				printf("Failed to read the input base address\n");
            sohoDevNum = *baseAddr;
            clean_stdin();
            *bus_interface = 1;

            /*Still need the USB init, not break here*/
        case '0':
        {
            int ret = 0;
            char buf[64];
            unsigned int usbId = 0;
            unsigned int usbPort = 0;

            do {
                printf("SMI Protocol select:\n");
                printf("\t0: MDIO(Clause22)\n");
                printf("\t1: XMDIO(Clause45), only support by Oak/Spruce/Fir...\n");
                printf("Please select one protocol: ");
                if(scanf("%c", (char*)(&command)) == 0)
					printf("Failed to read the input SMI Protocol index\n");
                clean_stdin();
                switch (command)
                {
                case '0':
                    gXMDIOFlag = 0;
                    break;
                case '1':
                    gXMDIOFlag = 1;
                    break;
                default:
                    gXMDIOFlag = 0xff;
                    break;
                }
            } while ((gXMDIOFlag != 0) && (gXMDIOFlag != 1));

            do {
                printf("Enter USB Port number(1 - 4):");
                if(scanf("%u", &usbPort) == 0)
					printf("Failed to read the input usb port\n");
                clean_stdin();
                setUSBPort(usbPort - 1);
            } while (!((usbPort > 0) && (usbPort <= 4)));

            printf("Only one USB Adapter connect(y/n)?");
            if(scanf(" %c", (char*)(&command)) == 0)
				printf("Failed to read the input\n");
            clean_stdin();
            if ((command == 'y') || (command == 'Y'))
            {
                ret = openDefaultUSBDriver();
                if (ret != 0)
                {
                    printf("Init USB Connect error, error code:%d\n", ret);
                    return -1;
                }
            }
            else
            {
                printf("Please enter the USB DID(0014 for example):");
                if(scanf("%x", &usbId) == 0)
					printf("Failed to read the input USD DID\n");
                clean_stdin();
                sprintf(buf, "0x%04x:Hi-Speed", usbId);
                ret = openUSBDriver(buf, usbPort - 1);
                if (ret != 0)
                {
                    printf("Init USB Connect error, error code:%d\n", ret);
                    return -1;
                }
            }
			if (*bus_interface < 0)
			{
				*bus_interface = 0;
			}

            break;
        }
        case '2':
        {
            int nicId = 0;
            int i = 0;
            int tmp_flag;

            do
            {
                tmp_flag = 0;
                printf("Select RMU Mode:\n");
                printf("\t0 : DSA Mode\n");
                printf("\t1 : Ether Type DSA Mode\n");
                printf("Please select one RMU mode: ");
                if(scanf(" %c", (char*)(&command)) == 0)
					printf("Failed to read the input RMU mode index\n");
                clean_stdin();
                switch (command)
                {
                case '0':
                    gRmuMode = MSD_RMU_DSA_MODE;
                    printf("Please enter the Target Device number (hex, Trg_Dev=0 please enter 0x0 for example):");
                    if(scanf("%x", (int*)&sohoDevNum) == 0)
						printf("Failed to read the input Target Device number\n");
                    break;
                case '1':
                    gRmuMode = MSD_RMU_ETHERT_TYPE_DSA_MODE;
                    printf("Please enter the Target Device number and Ether Type value\n(hex, Trg_Dev=0x0 and Etype=0x9100 please enter 0x0 0x9100 for example):");
					if (scanf("%x %x", (int*)&sohoDevNum, (int*)&gEtherTypeValue) == 0)
						printf("Failed to read input Target Devcie number and Ether Type\n");
                    clean_stdin();
                    break;
                default:
                    tmp_flag = 1;
                    break;
                }
            } while (tmp_flag == 1);

            pcap_rmuGetDeviceList();
            do {
                printf("\nPlease select NIC Index: ");
                if(scanf("%d", &nicId) == 0)
					printf("Failed to read the input NIC index\n");
				clean_stdin();
				if (!((nicId > 0) && (nicId <= NumOfNIC) && (nicId < 10)))
				{
					printf("Input nicId is invalid.\n");
					continue;
				}
                strcpy(pcap_device_name, AdapterList[nicId - 1]);
#ifdef WIN32
                IP_ADAPTER_INFO AdapterInfo[16];
                DWORD dwBuflen = sizeof(AdapterInfo);

                DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBuflen);
                PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

                do{
                    if (strstr(pcap_device_name, pAdapterInfo->AdapterName) != 0)
                    {
                        memcpy(AdapterMACAddr, pAdapterInfo->Address, 6);
                        break;
                    }
                    pAdapterInfo = pAdapterInfo->Next;
                } while (pAdapterInfo);
#else
                struct   ifreq   ifreq;
                int   sock;
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0)
                {
                    perror("socket ");
                    return   2;
                }
                strncpy(ifreq.ifr_name, pcap_device_name, IF_NAMESIZE - 1);
                if (ioctl(sock, SIOCGIFHWADDR, &ifreq) <0)
                {
                    close(sock);
                    perror("ioctl ");
                    return   3;
                }
                memcpy(AdapterMACAddr, ifreq.ifr_hwaddr.sa_data, 6);
#endif

            } while (!((nicId > 0) && (nicId <= NumOfNIC)));

            if (pcap_rmuOpenEthDevice() != 0)
            {
                printf("NIC Open Failed\n");
                printf("Press any key to exit program...");
                command = getchar();
                return -1;
            }
            *bus_interface = 2;
            break;
        }
		case '3':
		{
			int nicId = 0;

			gRmuMode = MSD_RMU_ETHERT_TYPE_DSA_MODE;
			printf("Please enter the Target Device number and Ether Type value\n(hex, Trg_Dev=0x0 and Etype=0x9101 please enter 0x0 0x9101 for example):");
			if (scanf("%x %x", (int*)&sohoDevNum, (int*)&gEtherTypeValue) == 0)
				printf("Failed to read input Target Devcie number and Ether Type\n");
			clean_stdin();

			gFWRmuFlag = 1;

			pcap_rmuGetDeviceList();
			do {
				printf("\nPlease select NIC Index: ");
				if (scanf("%d", &nicId) == 0)
					printf("Failed to read the input NIC index\n");
				clean_stdin();
				if (!((nicId > 0) && (nicId <= NumOfNIC) && (nicId < 10)))
				{
					printf("Input nicId is invalid.\n");
					continue;
				}
				strcpy(pcap_device_name, AdapterList[nicId - 1]);
#ifdef WIN32
				IP_ADAPTER_INFO AdapterInfo[16];
				DWORD dwBuflen = sizeof(AdapterInfo);

				DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBuflen);
				PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

				do{
					if (strstr(pcap_device_name, pAdapterInfo->AdapterName) != 0)
					{
						memcpy(AdapterMACAddr, pAdapterInfo->Address, 6);
						break;
					}
					pAdapterInfo = pAdapterInfo->Next;
				} while (pAdapterInfo);
#else
				struct   ifreq   ifreq;
				int   sock;
				if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0)
				{
					perror("socket ");
					return   2;
				}
				strncpy(ifreq.ifr_name, pcap_device_name, IF_NAMESIZE - 1);
				if (ioctl(sock, SIOCGIFHWADDR, &ifreq) <0)
				{
					close(sock);
					perror("ioctl ");
					return   3;
				}
				memcpy(AdapterMACAddr, ifreq.ifr_hwaddr.sa_data, 6);
#endif

			} while (!((nicId > 0) && (nicId <= NumOfNIC)));

			if (pcap_rmuOpenEthDevice() != 0)
			{
				printf("NIC Open Failed\n");
				printf("Press any key to exit program...");
				command = getchar();
				return -1;
			}
			*bus_interface = 3;
			break;
		}
        case '4':
        {
            printf("Please enter the fake device (88Q5152, 88Q5072, 88E6391X, 88E6390, 88E6320 ...)\n");
            char temp[20] = { 0 };
            if(scanf("%s", temp) == 0)
				printf("Failed to read the input deviceId\n");

            gFakeDevice = getDeviceValue(temp);
            *bus_interface = 4;
            break;
        }
        default:
            break;
        }
    } while (*bus_interface < 0);

    return 0;
}
static char* getCfgFile(int argc, char *argv[])
{
    char *filename = NULL;
    int index;

    char* cmd, *value;
    for (index = 1; index < argc;)
    {
        cmd = argv[index];
        index++;

        if (*cmd != '-')
        {
            printf("Invalid option input: %s\n", cmd);
            return NULL;
        }
        else
        {
            value = argv[index];
            index++;
        }

        if (strcmp(cmd, "-f") == 0)
        {
            filename = value;
        }
    }

    return filename;
}

int main(int argc, char *argv[])
{
    int ret;
    MSD_U32 baseAddr = 0;
    int bus_interface = -1;
    char *filename = NULL;
	MSD_U16 inputDeviceId = 0;
	MSD_STATUS status = 0;

    filename = getCfgFile(argc, argv);

    if (filename != NULL)
    {
		ret = openBusWithCfgFile(filename, &bus_interface, &baseAddr);
    }
    else
    {
		ret = openBusInterface(&bus_interface, &baseAddr);
    }

	if (ret != 0)
	{
		int command = 0;
		printf("Failed to open Bus Interface (USB/RMU)\n");
		printf("Do you want to exit program(y/Y)?");
		command = getchar();
		clean_stdin();
		if ((command == 'y') || (command == 'Y'))
		{
			return ret;
		}
		else
		{
			printf("Invalid input parameters, please use (y/Y) ...\n");
			if (scanf(" %c", (char*)(&command)) == 0);

			return 1;
		}
	}

	if (qdInit(baseAddr, bus_interface, inputDeviceId) != 0)
	{
        int command = 0;
		printf("QD Load Failed For Unkown Device\n\n");
		printf("Do you want to exit program(y/Y) or input device number(n/N)?");
		if (scanf(" %c", (char*)(&command)) == 0)
			printf("Failed to read the input info\n");
		clean_stdin();
		if ((command=='y')||(command=='Y'))
		{
			return 1;
		}
		else if ((command == 'n') || (command == 'N'))
		{
			do
			{
				printf("Please enter the device (88Q5152, 88Q5072, 88Q6113, 88E6393X ...)\n");
				char temp[20] = { 0 };
				if (scanf("%s", temp) == 0)
					printf("Failed to read the input deviceId\n");

                inputDeviceId = getDeviceValue(temp);
                if (bus_interface != 3)
                {
                    gFakeDevice = 0;
                }
                else
                {
                    gFakeDevice = inputDeviceId;
                }

				/* re-init device again for unprogramed device */
				status = qdInit(baseAddr, bus_interface, inputDeviceId);
				if (status != 0)
				{
					printf("Unkown input deviceId, exit program(y/Y) or input again(n/N)?");
					if (scanf(" %c", (char*)(&command)) == 0)
						printf("Failed to read the input info\n");
					clean_stdin();
					if ((command == 'y') || (command == 'Y'))
					{
						return 1;
					}
				}
				
			} while (status != 0);
		}
		else
		{
			printf("Invalid input parameters, please use (y/n) ...\n");
			if (scanf(" %c", (char*)(&command)) == 0);

			return 1;
		}
	}

	/* print SW name */
	char *swName = SWGetDeviceName((int)qddev->deviceId);
	printf("%s Driver Load Success\n", swName);


    if (filename == NULL)
    {
        printf("\nPlease enter the telnet port(8888 for example):\n");
        if(scanf("%hu", &g_telnet_port) == 0)
			printf("Failed to read input telnet port\n");
        clean_stdin();
    }

	/*apiCLI(); */
	cliCommand();

    pcap_rmuCloseEthDevice();

	qdClose();
	return 0;
}
