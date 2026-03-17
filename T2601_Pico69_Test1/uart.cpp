/*
TX #0: <##C1T1=;2026;03;17;09;12>
*/

#include "main.h"
#include "uart.h"
#include "atask.h"
#include "io.h"

typedef struct 
{
    int8_t free_indx;
    int8_t tx_indx;
} uart_ctrl_st;

typedef struct
{
    char msg[UART_MSG_LEN];
    bool updated;
    uint32_t  interval;
    uint32_t  next_send;
} uart_tx_st;

uart_ctrl_st uctrl = {0};

uart_tx_st uart_tx[UART_TX_BUFFERS] = {0};

void uart_tx_task(void);

atask_st tx_th                = {"UART TX Task   ", 1000,0, 0, 255, 0, 1, uart_tx_task};

void uart_initialize(void)
{
    uctrl.free_indx = 0;
    atask_add_new(&tx_th);
}

int8_t uart_reserve_tx_buffer(uint32_t ival)
{
    int8_t bindx = -1;
    if(uctrl.free_indx < UART_TX_BUFFERS) {
        uart_tx[uctrl.free_indx].interval = ival;
        uart_tx[uctrl.free_indx].next_send = millis() + ival;
        uart_tx[uctrl.free_indx].updated = false;
        bindx = uctrl.free_indx;
        uctrl.free_indx++;
    }
    return bindx;
}

void uart_add_msg(int8_t msg_indx, char *msg)
{
    if(msg_indx < uctrl.free_indx)
    {
        strncpy(uart_tx[msg_indx].msg, msg, UART_MSG_LEN);
        uart_tx[msg_indx].updated = true;
    }
}



void uart_tx_task(void)
{
    switch(tx_th.state)
    {
        case 0:
            uctrl.tx_indx = 0;
            tx_th.state = 10;
            break;
        case 10:
            if ((uart_tx[uctrl.tx_indx].updated) && 
                (millis() > uart_tx[uctrl.tx_indx].next_send))
            {
                uart_tx[uctrl.tx_indx].next_send = millis() + uart_tx[uctrl.tx_indx].interval;
                tx_th.state = 100;
            } 
            else tx_th.state = 50;
            break;
        case 50:
            if((uctrl.tx_indx+1) < uctrl.free_indx) uctrl.tx_indx++;
            else uctrl.tx_indx = 0;
            tx_th.state = 10;
            break;    
        case 100:
            Serial.printf("TX #%d: %s\n",uctrl.tx_indx, uart_tx[uctrl.tx_indx].msg);
            SerialTFT.println(uart_tx[uctrl.tx_indx].msg);
            uart_tx[uctrl.tx_indx].updated = false;
            tx_th.state = 50;
            break;
        case 200:
            tx_th.state = 10;
            break;
    }
}