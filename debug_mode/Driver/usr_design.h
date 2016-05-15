#ifndef _USR_DESIGN_H_
#define _USR_DESIGN_H_
#include "products.h"

extern unsigned char macAddr[6];
extern unsigned char softVersion[12];		//software version	V1.0
extern unsigned char hardVersion[12];		//hardware version	V1.0
extern unsigned char serverIPAddr[4];		//client ip 192.168.0.1
extern unsigned char serverPort[2];			//client port 8080
extern unsigned char clientIPAddr[4];	//client ip 192.168.0.1
extern unsigned char netmaskAddr[4];
extern unsigned char gwAddr[4];
extern unsigned char clientPort[2];			//client port 8080
extern unsigned char clientPort[2];			//client port 8080
extern unsigned char usrtMap[4][2];

extern unsigned char LinkStatus;

void versionInit(unsigned char sa,unsigned char sb,unsigned char sc,unsigned char sd,
	unsigned char ha,unsigned char hb,unsigned char hc,unsigned char hd);
void MacInit(void) ;
void usrtMapInit(void);
void localIpPortInit(void);
void serverIpPortInit(void);
unsigned char TcpIpLinkStatus(void);
void receive_server_packet(unsigned char *data,unsigned short length);
void usr_receive_server_timer(unsigned short const msgid);
void usr_system_reset_timer(unsigned short const msgid);
void usr_tcp_ip_link_timer(unsigned short const msgid);
void display_info(void);
void usr_tcpip_timeout_timer(unsigned short const msgid);
void usr_heartbeat_cmd(unsigned short const timeout);

#endif

