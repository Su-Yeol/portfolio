#ifndef __BUSCONF_H_
#define __BUSCONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "msdApi.h"

typedef struct
{
    MSD_BOOL defaultUsb;
    unsigned int usb_port;
    unsigned int usb_did;
    MSD_U8 xmdioFlag;
}SMI_STRUCT;

typedef struct
{
    MSD_RMU_MODE rmu_mode;
    MSD_U32 ethertype;
    char nic_name[100];
}RMU_STRUCT;

typedef struct
{
    MSD_INTERFACE bus_interface;
    MSD_U8 dev_num;
    SMI_STRUCT smi;
    RMU_STRUCT rmu;
}BUS_CONF_STRUCT;

typedef struct conf_list
{
    struct conf_list *prev, *next,
        *child, *parent;

    char *name, *value;
}CONF_LIST;

int bus_conf_parse(char* filename, BUS_CONF_STRUCT* bus_conf);

#ifdef __cplusplus
}
#endif

#endif