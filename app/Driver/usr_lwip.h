#ifndef _USR_LWIP_H_
#define _USR_LWIP_H_
#include "stm32f10x.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h"
#include "stm32_eth.h"
#include <stdio.h>


void LWIP_Init(void);
void lwip_periodic_handle(void);

#endif

