#ifndef __MAIN_H__
#define __MAIN_H__
#include "WString.h"
#define   __APP__ ((char*)"T2603_PicoRuuviRfm69")

#define DEBUG_PRINT 
#define T2601_PICO_RFM69
#undef ENABLE_RFM69
#include <Arduino.h>
#include "rfm69.h"

#define MY_MODULE_TAG   'R'
#define MY_MODULE_ADDR  '1'

typedef struct
{
    uint32_t next_io_tick;
} main_ctrl_st;

typedef struct
{
    char            tag;
    char            addr;         
} modem_data_st;




#endif