#include <stdlib.h>
#include "main.h"
#include "handler.h"
#include "uart.h"
#include "atask.h"

#define  NBR_OF_EVENTS     4
#define  MSG_TAG_LEN       8
#define  MSG_MAX_FIELD_NBR 16
#define  MSG_FIELD_LEN     16


typedef enum
{
    MSG_MATCH_DATE_TIME = 0,
    MSG_MATCH_NBR_OF
} msg_match_et;

typedef struct 
{
    char        tag[MSG_TAG_LEN];
    char        label[13];
    char        value[13];
    int16_t     rssi;
} event_radio_msg_st;


char msg_field[MSG_MAX_FIELD_NBR][MSG_FIELD_LEN] = {};




typedef struct
{
    event_radio_msg_st msg;
    uint8_t     severity;
    uint8_t     state;
    uint8_t     new_val;
    uint8_t     prev_val;
    uint32_t    timeout;
    uint32_t    last_update;
    uint32_t    next_update;
    uint32_t    alarm_timeout;
    uint32_t    wait_timeout;
} event_st;


typedef struct
{
     char  zone[MSG_FIELD_LEN];
     char  item[MSG_FIELD_LEN];
     char  value[MSG_FIELD_LEN];
} node_field_values_st;

typedef struct
{
    uint16_t    year;
    uint8_t     month;
    uint8_t     day;
    uint8_t     hour;
    uint8_t     minute;
} date_time_st;

typedef struct
{
    char  match[MSG_FIELD_LEN];
} msg_match_st;


// typedef struct
// {
//     uint8_t state;
//     uint32_t alarm_timeout;
//     uint32_t wait_timeout;
// } node_state_machine_st;

// typedef struct
// {
//     node_field_values_st fields;
//     node_type_et type;
//     int16_t rssi;
//     uint32_t last_update;
//     node_state_machine_st sm; 
// } node_st;


typedef struct
{
    int16_t     rssi;
    uint8_t     nbr_fields;
    date_time_st    date_time;
    uint8_t     alarm_level;
    uint8_t     prev_alarm_level;
    uint32_t    timeout;
    uint8_t     relay_module_indx;
    uint8_t     relay_indx;
    uint8_t     opto_indx;
    uint32_t    radiate_timeout;
} handler_ctrl_st;



msg_match_st msg_match[MSG_MATCH_NBR_OF] =
{
    [MSG_MATCH_DATE_TIME] = {.match="##C*T*="},
};
    

handler_ctrl_st hctrl = 
{
    .rssi = 0,
    .nbr_fields = 0,
    .date_time = {0,0,0,0,0},
    .timeout = 0,
    .relay_module_indx = 0,
    .relay_indx = 0,
    .opto_indx = 0,
    // .decoded_opto = {
    //     RELAY_MODULE_TAG, RELAY_MODULE_ADDR, 
    //     MY_MODULE_TAG, MY_MODULE_ADDR,
    //     OPTO_FUNCTION, WILD_CHAR,
    //     ACTION_GET, WILD_CHAR
    // },
    // .decoded_relay = {
    //     RELAY_MODULE_TAG, RELAY_MODULE_ADDR, 
    //     MY_MODULE_TAG, MY_MODULE_ADDR,
    //     RELAY_FUNCTION, '0',
    //     ACTION_SET, WILD_CHAR
    // },
    // .decoded_rec = {'*','*','*','*','*','*','*','*'},
    .radiate_timeout = 0
};

// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
//atask_st h_handle           = {"Handler Task   ", 100,0, 0, 255, 0, 1, handler_task};


void handler_initialize(void)
{
    // atask_add_new(&h_handle);
}



uint8_t handler_find_match(char *msg_tag)
{
    uint8_t match_indx = 0;
    uint8_t char_indx = 0;
    bool do_continue_match = true;
    bool do_continue_char = true;
    bool match_found = false;
    uint8_t len = strlen(msg_tag);

    while(do_continue_match && !match_found)
    {
        do_continue_char = true;
        char_indx = 0;
        Serial.printf("%s =?= %s\n",msg_tag,msg_match[match_indx].match);
        while(do_continue_char)
        {
            if(strlen(msg_match[match_indx].match) == len)
            {
                if(msg_match[match_indx].match[char_indx] != '*')
                {
                    if(msg_match[match_indx].match[char_indx] != msg_tag[char_indx])
                        do_continue_char = false;
                }
                char_indx++;
                if(char_indx >= len) {
                    do_continue_char = false;    
                    match_found = true;
                }
            }
            else do_continue_char = false;
        }
        if(!match_found){
            match_indx++;
            if(match_indx >= MSG_MATCH_NBR_OF) do_continue_match= false;
        }
    }

    return match_indx;
}

void handler_print_fields(void)
{
    Serial.printf("Number of fields: %d : ",hctrl.nbr_fields);
    for(uint8_t i = 0; i < hctrl.nbr_fields; i++)
    {
        Serial.printf("%s ",msg_field[i]);
    }
    Serial.println();
}


void handler_print_date_time(date_time_st *date_time_p)
{
    Serial.printf("%d-%d-%d %d:%d",
        date_time_p->year ,
        date_time_p->month ,
        date_time_p->day ,
        date_time_p->hour ,
        date_time_p->minute);
}
void handler_process_fields(void)
{
    uint8_t mindx =handler_find_match(msg_field[0]);
    Serial.printf("Match Index %d\n",mindx);
    switch(mindx)
    {
        case MSG_MATCH_DATE_TIME:
            hctrl.date_time.year    = (uint16_t) strtoul(msg_field[1], NULL,10);
            hctrl.date_time.month   = (uint8_t) strtoul(msg_field[2], NULL,10);
            hctrl.date_time.day     = (uint8_t) strtoul(msg_field[3], NULL,10);
            hctrl.date_time.hour    = (uint8_t) strtoul(msg_field[4], NULL,10);
            hctrl.date_time.minute  = (uint8_t) strtoul(msg_field[5], NULL,10);
            handler_print_date_time(&hctrl.date_time);
            break;
    }
}
bool handler_split_msg(char *msg, int16_t rssi )
{
    bool do_continue = true;
    bool all_done = false;
    String Msg = msg;
    String Sub;
    int indx1 = 1;
    int indx2 = Msg.indexOf(';');
    int indx_end = Msg.indexOf('>');
    uint8_t findx = 0;   // field index

    hctrl.nbr_fields = 0;
    hctrl.rssi = rssi;
    //hctrl.rssi = rssi;
    Msg.trim();
    uint8_t len = Msg.length();
    if(Msg[0] != '<') do_continue = false;
    if(Msg[len-1] != '>') do_continue = false;
    if(indx_end < 0 )  do_continue = false;
    if(!do_continue) Serial.println("Frame was NOK");
    if(indx2 < 2) {
        Serial.println("Message is to short!");
        do_continue = false;
    }

    if(do_continue){
        while(!all_done && do_continue)
        {
            Sub = Msg.substring(indx1,indx2);
            Sub.toCharArray(msg_field[findx], MSG_FIELD_LEN );
            indx1 = indx2+1;
            indx2 = Msg.indexOf(';',indx1+1);
            if(indx2 < 0){
                if(indx1 < indx_end ) indx2 = indx_end;
                else all_done = true;
            }
            findx++;
        }
    }

    if (do_continue) {
        hctrl.nbr_fields = findx;
        handler_print_fields();
        handler_process_fields();
    }
    return do_continue;
}
