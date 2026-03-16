
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------

typedef enum
{
    NODE_TYPE_UNDEF = 0,
    NODE_TYPE_PIR,
    NODE_TYPE_DOOR,
    NODE_TYPE_TEMPERATURE,
    NODE_TYPE_NBR_OF,
} node_type_et;

*********************************************************************************************************

#ifndef __ALARM_H__
#define __ALARM_H__

// <ALARM;A1;3;Piha>
typedef struct
{
    uint8_t     severity;
    char        rem[FIELD_LEN];
    uint16_t     state;
    uint32_t    interval;
    uint32_t    duration;
    uint32_t    timeout;
} alarm_test_msg_st;

void alarm_initialize(void);

#endif
---------------------------------------------------------------------------------------------------------
#include "main.h"
#include "Rfm69Modem.h"
#include "handler.h"
#include "alarm.h"
#include "atask.h"

typedef struct 
{
    Rfm69Modem  *modem;
    uint32_t    timeout;
} alarm_ctrl_st;

extern modem_data_st   modem_data;

void alarm_task(void);

// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st a_handle           = {"Alarm   Task   ", 100,0, 0, 255, 0, 1, alarm_task};


alarm_ctrl_st actrl;
// Alarm Message
// <ALARM;A1;3;Piha>
alarm_test_msg_st test_alarm[4] = 
{
    {.severity=3, .rem="Piha1", .state=0, .interval=44000, .duration=10000, .timeout=0},
    {.severity=3, .rem="Piha2", .state=0, .interval=64000, .duration=8000, .timeout=0},
    {.severity=3, .rem="Tera",  .state=0, .interval=111000, .duration=10000, .timeout=0},
    {.severity=3, .rem="LA",    .state=0, .interval=133000, .duration=10000, .timeout=0},
};

void alarm_initialize(void)
{
    //actrl.modem = rfm69_modem;
    atask_add_new(&a_handle);
}
// <ALARM;A1;3;Piha>
void alarm_send(uint8_t severity, char *rem )
{
    char msg[40];
    sprintf(msg,"<ALARM;%c%c;%d;%s>",
        modem_data.tag,
        modem_data.addr,
        severity,
        rem);
    Serial.println(msg);
    //actrl.modem->radiate(msg);
}

bool alarm_test_state_machine(alarm_test_msg_st *atp)
{
    bool did_radiate = false;
    switch(atp->state)
    {
        case 0:
            atp->state = 10;
            break;
        case 10:
            atp->state = 20;
            atp->timeout = millis()+atp->interval;
        case 20:
            if (millis() > atp->timeout){
                alarm_send(atp->severity, atp->rem);
                did_radiate = true;
                atp->timeout = millis()+atp->duration;
                atp->state = 30;
            }
            break;
        case 30:
            if (millis() > atp->timeout){
                alarm_send(0, atp->rem);
                did_radiate = true;
                atp->state = 10;
            }
            break;
        case 40:
            atp->state = 10;
            break;
    }
    return did_radiate;
}

void alarm_task(void)
{
    static uint8_t  test_indx = 0;

    switch(a_handle.state)
    {
        case 0:
            a_handle.state = 10;
            break;
        case 10:
            if(test_indx >= 4) test_indx=0;
            if (alarm_test_state_machine(&test_alarm[test_indx])){
                a_handle.state = 20;
                actrl.timeout = millis() + 2000;
            }
            test_indx++;
            break;
        case 20:
            if (millis() > actrl.timeout) a_handle.state = 10;
            break;
        case 30:
            a_handle.state = 10;
            break;

    }
}

*********************************************************************************************************


#define NBR_TEST_MSG  4
#define LEN_TEST_MSG  32
const char test_msg[NBR_TEST_MSG][LEN_TEST_MSG] =
{  //12345678901234567890123456789012
    "<R1X1J1:Dock;T_bmp1;9.1;->",
    "<R1X1J1:Dock;T_dht22;8.7;->",
    "<R1X1J1:Dock;T_Water;5.3;->",
    "<R1X1J1:Dock;ldr1;0.33;->",
};
void send_test_data_task(void);
atask_st send_test_data_handle     = {"Send Test Task ", 10000,0, 0, 255, 0, 1, send_test_data_task};


void send_test_data_task(void)
{
    static uint8_t mindx = 0;
    Serial.println(test_msg[mindx]);
    SerialTFT.println(test_msg[mindx]);
    if(++mindx >= NBR_TEST_MSG) mindx = 0;
}


******************************************************************************************************

// function prototypes
void handler_task(void);


// atask_st modem_handle    = {"Radio Modem    ", 100,0, 0, 255, 0, 1, modem_task};
atask_st h_handle           = {"Handler Task   ", 100,0, 0, 255, 0, 1, handler_task};




event_st event[NBR_OF_EVENTS] = 
{
    {{"PIR","Piha1","xx",0},  3,0,0,0,0,0,0,0,0},
    {{"PIR","Piha2","xx",0},  5,0,0,0,0,0,0,0,0},
    {{"PIR","Ranta1","xx",0}, 6,0,0,0,0,0,0,0,0},
    {{"PIR","Ranta2","xx",0}, 1,0,0,0,0,0,0,0,0},
};





// node_st node[NBR_OF_NODES] =
// {
//   {{"Piha","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
//   {{"Piha","PIR2","xxxx"}, NODE_TYPE_PIR, 0},
//   {{"LA","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
//   {{"Tera","PIR1","xxxx"}, NODE_TYPE_PIR, 0},
// };


event_radio_msg_st event_model = {"EVENT", "xxxxx", "zz",0};
event_radio_msg_st rec_event = {"XXX", "xxxxx", "42",0};



void handler_print_event(event_radio_msg_st *emsg)
{
    Serial.printf("Message tag: %s", emsg->tag);
    Serial.printf(" Label: %s", emsg->label);
    Serial.printf(" Value: %s", emsg->value);
    Serial.printf(" RSSI: %s", emsg->value);
    Serial.println();
}

void handler_process_event(event_radio_msg_st *ev)
{
    uint8_t indx = 0;
    bool    found = false;
    // String Field;
    int     pos;

    while(!found && indx < NBR_OF_NODES)
    {
        if (strcmp(ev->tag, event[indx].msg.tag) == 0){
            Serial.println("Tag was identified");
            if (strcmp(ev->label, event[indx].msg.label) == 0){
                Serial.print("Label was identified, index="); Serial.println(indx);
                found=true;
            }
        }
    }
}


bool handler_parse_msg(char *msg, int16_t rssi )
{
    bool do_continue = true;
    String Msg = msg;
    String Sub;
    int indx1 = 1;
    int indx2 = Msg.indexOf(';');
    rec_event.rssi = rssi;
    //hctrl.rssi = rssi;
    Msg.trim();
    uint8_t len = Msg.length();
    if(Msg[0] != '<') do_continue = false;
    if(Msg[len-1] != '>') do_continue = false;
    if (!do_continue) Serial.println("Frame was NOK");
    

    if (indx2 < 2) do_continue = false;
    if (do_continue) {
        Sub = Msg.substring(indx1,indx2);
        Sub.toCharArray(rec_event.tag, MSG_TAG_LEN );
        indx1 = indx2+1;
        indx2 = Msg.indexOf(';',indx1+1);
    }
    // if (do_continue) {
    //     Sub = Msg.substring(indx1,indx2);
    //     Sub.toCharArray(rec_event.label, MSG_LABEL_LEN );
    //     indx1 = indx2+1;
    //     indx2 = Msg.indexOf('>',indx1+1);
    // }
    if (do_continue) {
        Sub = Msg.substring(indx1,indx2);
        Sub.toCharArray(rec_event.value, MSG_FIELD_LEN );
    }

    if (do_continue) {
        handler_print_event(&rec_event);
        //handler_process_event(&rec_event);
    }
    return do_continue;
}

void handler_node_state_machine(event_st *node)
{
    switch(node->state)
    {
        case 0:
            node->state = 10;
            break;
        case 10:
            if(node->msg.value[0]=='1'){
                node->state = 20;
                node->alarm_timeout = millis() + 5000;
            } 
            break;
        case 20:
            if(millis() > node->alarm_timeout){
                node->wait_timeout = millis() + 30000;
                node->state = 30;
            }
            break;
        case 30:
            if(millis() > node->wait_timeout){
                node->state = 10;
            }
            break;
        case 40:
            node->state = 10;
            break;
        case 50:
            node->state = 10;
            break;
    }
}

void handler_debug_print(void)
{
    Serial.println("Events: ");
    for(uint8_t indx = 0; indx < NBR_OF_NODES; indx++)
    {
        Serial.printf("%s-%s=%s",
            event[indx].msg.tag , 
            event[indx].msg.label, 
            event[indx].msg.value);
        if (event[indx].alarm_timeout > millis()) Serial.println("=on ");   
        else Serial.println("=off ");      
    }
}

void handler_task(void)
{
    static boolean alarm_is_active;
    uint8_t active_cntr = 0;

    for(uint8_t indx = 0; indx < NBR_OF_NODES; indx++)
    {
        handler_node_state_machine(&event[indx]);
        if (event[indx].alarm_timeout > millis()) active_cntr++;        
    }


    switch (h_handle.state)
    {
        case 0:
            h_handle.state = 10;
            break;
        case 10:
            if (active_cntr > 0) 
            {
                h_handle.state = 20;
                hctrl.timeout = millis() + 10000;
                Serial.printf("Alarm On! active = %d\n", active_cntr );
            }
            break;
        case 20:
            if( millis() > hctrl.timeout)
            {
                h_handle.state = 30;
                Serial.printf("Alarm Off! active = %d\n", active_cntr);
                hctrl.timeout = millis() + 5000;
            }
            break;
        case 30:
            if( millis() > hctrl.timeout) {
                h_handle.state = 10;
            }
            break;

    }
    
}
