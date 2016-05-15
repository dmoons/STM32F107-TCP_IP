#ifndef  _TCP_CLIENT_H_
#define  _TCP_CLIENT_H_

#include "lwip/tcp.h"

extern struct tcp_pcb *tcp_client_pcb;
extern struct tcp_pcb *tcp_active_pcbs;

void TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char *ipBuf);
err_t TCP_Client_Send_Data(unsigned char *buff,unsigned int length);

#endif
