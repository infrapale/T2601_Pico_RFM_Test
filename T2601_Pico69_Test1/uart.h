#ifndef __UART_H__
#define __UART_H__

#define UART_MSG_LEN        40
#define UART_TX_BUFFERS     8

typedef struct
{
    char to_tag;
    char to_addr;
    char from_tag;
    char from_addr;
    char function;
    char func_indx;
    char action;
    char value;
} uart_msg_st;

void uart_initialize(void);

int8_t uart_reserve_tx_buffer(uint32_t ival);

void uart_add_msg(int8_t msg_indx, char *msg);


#endif