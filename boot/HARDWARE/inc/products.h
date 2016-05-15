#ifndef _PRODUCTS_H_
#define _PRODUCTS_H_

#define DEVELOPER_MODE		0
#define NORMAL_MODE			1

#define BOARD_MODE			NORMAL_MODE


#define DEBUG	(1)

#if DEBUG

#define USART1_ENABLE	(1)
#define USART2_ENABLE	(0)

#else

#define QPRINTF(fmt, ...)

#endif



#endif

