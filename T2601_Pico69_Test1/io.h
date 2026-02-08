#ifndef __IO_H__
#define __IO_H__

#define SerialTFT       Serial1
#define T2601_PICO_RFM69

#define RFM69_CS      17
#define RFM69_INT     21
#define RFM69_RST     20

// LED Definitions
#define LED_NBR_OF      2

#define PIN_TX0         (0u)
#define PIN_RX0         (1u)

#define PIN_I2C1_SDA    (2u)
#define PIN_I2C1_SCL    (3u)

#define PIN_TX1         (4u)
#define PIN_RX1         (5u)

#define PIN_LED_RED     (6u)
#define PIN_LED_BLUE    (7u)

#define PIN_DIP_SW1     (8u)
#define PIN_DIP_SW2     (9u)
#define PIN_DIP_SW3     (10u)
#define PIN_DIP_SW4     (11u)
#define PIN_DIP_SW5     (12u)
#define PIN_DIP_SW6     (13u)
#define PIN_DIP_SW7     (14u)
#define PIN_DIP_SW8     (15u)
#define PIN_RFM_MISO    (16u)
#define PIN_RFM_CS      (17u)
#define PIN_RFM_SCK     (18u)
#define PIN_RFM_MOSI    (19u)
#define PIN_RFM_RESET   (20u)
#define PIN_RFM_IRQ     (21u)
#define PIN_RUN_RFM     (22u)
#define PIN_LDR_AN      (26u)
#define PIN_ABTN        (27u)



typedef enum
{
    LED_INDX_RED =0,
    LED_INDX_BLUE,
    LED_INDX_NBR_OF
} led_index_et;

void io_initialize(void);

void io_led_flash(led_index_et led_indx, uint16_t nbr_ticks );

void io_run_100ms(void);


#endif