/*
 * USB.h
 *
 *  Created on: Dec 9, 2013
 *      Author: jwmcclur
 */

#ifndef USB_H_
#define USB_H_

extern void UARTinit(void);

extern bool BT_UART_CharsAvail(void);
extern void BT_UART_CharPut(char);
extern char BT_UART_CharGet(void);
extern char BT_UART_StrPut(char *);
extern void BT_UART_NLPut(void);
extern void BT_NLPut(void);
extern void BT_NumPut( unsigned int z);

void BT_CMD_INTERP( char * cptr );
void BT_UART_Echo(void);

#endif /* USB_H_ */
