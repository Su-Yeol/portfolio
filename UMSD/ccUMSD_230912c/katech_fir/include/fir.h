#ifndef FIR_H // 02.14
#define FIR_H // 02.14

typedef int SOCKET;
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;

#define SOCKET_OK 0
#define SOCKET_ERROR -1
#define MCLI_PORT_DEFAULT 8888

/* Refer to the section, FRER of endpoint.ini */
enum {
	FIR_FRER_NONE,
	FIR_FRER_REPLICATION,
	FIR_FRER_ELIMINATION
};

int openavbFirInitialize(unsigned port_left, unsigned port_right, unsigned port_bottom, unsigned frer_fn);
void openavbFirFinalize(void);

int fir_connect(void);
int fir_send_data(char *data, int len);

int fir_cfg_srp(char *macAddr, U16 qPri1Rate);
int fir_cfg_frer_rep(char *macAddr);
int fir_cfg_frer_eli(char *macAddr);

#endif // 02.14
