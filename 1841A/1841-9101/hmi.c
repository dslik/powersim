// ---------------------------------------------------------------------------------
// Front panel display routines
// ---------------------------------------------------------------------------------
// Routines for displaying user interfaces on various power simulator front panels
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "hmi.h"
#include "fonts.h"
#include "pins.h"
#include "vita40.h"
#include "asm_hmi.h"
#include "st7789_lcd.h"
#include "snon/snon_utils.h"
#include "pico-utils/ws2812.h"

// Defines
#define FRONT_PANEL_LED_COUNT   108
#define DRAW_TITLE              0b00000001
#define DRAW_PHASE_1            0b00000010
#define DRAW_PHASE_2            0b00000100
#define DRAW_PHASE_3            0b00001000

// Private prototypes
void draw_gen_top_init(void);
void draw_gen_top(uint8_t update_region);
void draw_gen_bottom_init(void);
void draw_gen_bottom(uint8_t update_region);
uint32_t vita40_to_urgb(char* vita40_state, uint32_t counter);

// Global variables
uint32_t        led_update_counter = 0;
extern uint     pio_sm;
extern uint     pio_sm_offset;
char*           prev_title_time = NULL;
char*           prev_phase1v_time = NULL;
char*           prev_phase2v_time = NULL;
char*           prev_phase3v_time = NULL;
char*           prev_phase1c_time = NULL;
char*           prev_phase2c_time = NULL;
char*           prev_phase3c_time = NULL;

// Functions
void init_gen_screens(void)
{
    draw_gen_top_init();
    draw_gen_bottom_init();
}

void draw_gen_top_init(void)
{
    st7789_start_pixels(PIN_CS_1);

    // Subtitle area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred("L1", B612_BMA_24, (SCREEN_WIDTH / 3) * 0, (SCREEN_WIDTH / 3) * 1, 160);
    st7789_draw_string_centred("L2", B612_BMA_24, (SCREEN_WIDTH / 3) * 1, (SCREEN_WIDTH / 3) * 2, 160);
    st7789_draw_string_centred("L3", B612_BMA_24, (SCREEN_WIDTH / 3) * 2, (SCREEN_WIDTH / 3) * 3, 160);

    // Flow area
    asm_draw_flow_arrow(1, asm_flow_none);
    asm_draw_flow_arrow(2, asm_flow_none);
    asm_draw_flow_arrow(3, asm_flow_none);

    st7789_end_pixels();

    // Don't display voltages until values received
    prev_phase1v_time = snon_get_time("L1 Voltage");
    prev_phase2v_time = snon_get_time("L2 Voltage");
    prev_phase3v_time = snon_get_time("L3 Voltage");
}

void draw_gen_bottom_init(void)
{
    st7789_start_pixels(PIN_CS_2);

    // Subtitle area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred("L1", B612_BMA_24, (SCREEN_WIDTH / 3) * 0, (SCREEN_WIDTH / 3) * 1, 160);
    st7789_draw_string_centred("L2", B612_BMA_24, (SCREEN_WIDTH / 3) * 1, (SCREEN_WIDTH / 3) * 2, 160);
    st7789_draw_string_centred("L3", B612_BMA_24, (SCREEN_WIDTH / 3) * 2, (SCREEN_WIDTH / 3) * 3, 160);

    st7789_end_pixels();

    // Don't display currents until values received
    prev_phase1c_time = snon_get_time("L1 Current");
    prev_phase2c_time = snon_get_time("L2 Current");
    prev_phase3c_time = snon_get_time("L3 Current");
}

void update_gen_screens(void)
{
    char* value_time = NULL;

    value_time = snon_get_time("=W01=PHA01");

    if(value_time != prev_title_time)
    {
        draw_gen_top(DRAW_TITLE);
        draw_gen_bottom(DRAW_TITLE);
        prev_title_time = value_time;
    }

    value_time = snon_get_time("L1 Voltage");

    if(value_time != prev_phase1v_time)
    {
        draw_gen_top(DRAW_PHASE_1);
        prev_phase1v_time = value_time;
    }

    value_time = snon_get_time("L2 Voltage");

    if(value_time != prev_phase2v_time)
    {
        draw_gen_top(DRAW_PHASE_2);
        prev_phase2v_time = value_time;
    }

    value_time = snon_get_time("L3 Voltage");

    if(value_time != prev_phase3v_time)
    {
        draw_gen_top(DRAW_PHASE_3);
        prev_phase3v_time = value_time;
    }

    value_time = snon_get_time("L1 Current");

    if(value_time != prev_phase1c_time)
    {
        draw_gen_bottom(DRAW_PHASE_1);
        prev_phase1c_time = value_time;
    }

    value_time = snon_get_time("L2 Current");

    if(value_time != prev_phase2c_time)
    {
        draw_gen_bottom(DRAW_PHASE_2);
        prev_phase2c_time = value_time;
    }

    value_time = snon_get_time("L3 Current");

    if(value_time != prev_phase3c_time)
    {
        draw_gen_bottom(DRAW_PHASE_3);
        prev_phase3c_time = value_time;
    }

//    draw_gen_top();
//    draw_gen_bottom();


}

void draw_gen_top(uint8_t update_region)
{
    st7789_start_pixels(PIN_CS_1);

    // Title area
    if(update_region & DRAW_TITLE)
    {
        st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
        st7789_draw_string_centred(snon_get_value("=W01=PHA01"), B612_BMA_32, 0, SCREEN_WIDTH, 190);
    }

    // Value indicators
    if(update_region & DRAW_PHASE_1)
    {
        asm_draw_value_indicator(1, snon_get_value_as_double("L1 Voltage LoLo"),
                                    snon_get_value_as_double("L1 Voltage Lo"),
                                    snon_get_value_as_double("L1 Voltage"),
                                    snon_get_value_as_double("L1 Voltage Hi"),
                                    snon_get_value_as_double("L1 Voltage HiHi"),
                                    snon_get_value_as_double("L1 Voltage SP"));

        asm_draw_flow_value(1, snon_get_value_as_double("L1 Voltage"), "V");
    }

    if(update_region & DRAW_PHASE_2)
    {
        asm_draw_value_indicator(2, snon_get_value_as_double("L2 Voltage LoLo"),
                                    snon_get_value_as_double("L2 Voltage Lo"),
                                    snon_get_value_as_double("L2 Voltage"),
                                    snon_get_value_as_double("L2 Voltage Hi"),
                                    snon_get_value_as_double("L2 Voltage HiHi"),
                                    snon_get_value_as_double("L2 Voltage SP"));

        asm_draw_flow_value(2, snon_get_value_as_double("L2 Voltage"), "V");
    }

    if(update_region & DRAW_PHASE_3)
    {
        asm_draw_value_indicator(3, snon_get_value_as_double("L2 Voltage LoLo"),
                                    snon_get_value_as_double("L2 Voltage Lo"),
                                    snon_get_value_as_double("L2 Voltage"),
                                    snon_get_value_as_double("L2 Voltage Hi"),
                                    snon_get_value_as_double("L2 Voltage HiHi"),
                                    snon_get_value_as_double("L2 Voltage SP"));

        asm_draw_flow_value(3, snon_get_value_as_double("L3 Voltage"), "V");
    }

    //asm_draw_value_alarm(2, asm_alarm_one);
    //asm_draw_value_alarm(3, asm_alarm_two);

    st7789_end_pixels();
}

void draw_gen_bottom(uint8_t update_region)
{
    st7789_start_pixels(PIN_CS_2);

    if(update_region & DRAW_TITLE)
    {
        // Title area
        st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
        st7789_draw_string_centred(snon_get_value("=W01=PHA01"), B612_BMA_32, 0, SCREEN_WIDTH, 190);
    }

    if(update_region & DRAW_PHASE_1)
    {
        // Value indicators
        asm_draw_value_indicator(1, snon_get_value_as_double("L1 Current LoLo"),
                                    snon_get_value_as_double("L1 Current Lo"),
                                    snon_get_value_as_double("L1 Current"),
                                    snon_get_value_as_double("L1 Current Hi"),
                                    snon_get_value_as_double("L1 Current HiHi"),
                                    snon_get_value_as_double("L1 Current SP"));

        asm_draw_flow_value(1, snon_get_value_as_double("L1 Current"), "A");

        // Flow area
        if(snon_get_value_as_double("L1 Current") >=0)
        {
            asm_draw_flow_arrow(1, asm_flow_up);
        }
        else
        {
            asm_draw_flow_arrow(1, asm_flow_down);
        }
    }

    if(update_region & DRAW_PHASE_2)
    {
        // Value indicators
        asm_draw_value_indicator(2, snon_get_value_as_double("L2 Current LoLo"),
                                    snon_get_value_as_double("L2 Current Lo"),
                                    snon_get_value_as_double("L2 Current"),
                                    snon_get_value_as_double("L2 Current Hi"),
                                    snon_get_value_as_double("L2 Current HiHi"),
                                    snon_get_value_as_double("L2 Current SP"));

        asm_draw_flow_value(2, snon_get_value_as_double("L2 Current"), "A");

        // Flow area
        if(snon_get_value_as_double("L2 Current") >=0)
        {
            asm_draw_flow_arrow(2, asm_flow_up);
        }
        else
        {
            asm_draw_flow_arrow(2, asm_flow_down);
        }
    }

    if(update_region & DRAW_PHASE_3)
    {
        asm_draw_value_indicator(3, snon_get_value_as_double("L2 Current LoLo"),
                                    snon_get_value_as_double("L2 Current Lo"),
                                    snon_get_value_as_double("L2 Current"),
                                    snon_get_value_as_double("L2 Current Hi"),
                                    snon_get_value_as_double("L2 Current HiHi"),
                                    snon_get_value_as_double("L2 Current SP"));

        // Value area
        asm_draw_flow_value(3, snon_get_value_as_double("L3 Current"), "A");

        // Flow area
        if(snon_get_value_as_double("L3 Current") >=0)
        {
            asm_draw_flow_arrow(3, asm_flow_up);
        }
        else
        {
            asm_draw_flow_arrow(3, asm_flow_down);
        }
    }

    //asm_draw_value_alarm(2, asm_alarm_one);
    //asm_draw_value_alarm(3, asm_alarm_two);

    st7789_end_pixels();
}


uint32_t vita40_to_urgb(char* vita40_state, uint32_t counter)
{
    if(strcmp(vita40_state, "vita40_off") == 0)
    {
        return(urgb_u32(vita40_off));
    }

    if(strcmp(vita40_state, "vita40_red_steady") == 0)
    {
        return(urgb_u32(vita40_red_steady));
    }

    if(strcmp(vita40_state, "vita40_white_fast") == 0)
    {
        if(counter % 2 == 0)
        {
            return(urgb_u32(vita40_white_fast));
        }
        else
        {
            return(urgb_u32(vita40_off));
        }
    }

    if(strcmp(vita40_state, "vita40_blue_steady") == 0)
    {
        return(urgb_u32(vita40_blue_steady));
    }

    if(strcmp(vita40_state, "vita40_amber_slow") == 0)
    {
        if((counter / 5) % 2 == 0)
        {
            return(urgb_u32(vita40_amber_slow));
        }
        else
        {
            return(urgb_u32(vita40_off));
        }
    }

    if(strcmp(vita40_state, "vita40_amber_steady") == 0)
    {
        return(urgb_u32(vita40_amber_steady));
    }

    if(strcmp(vita40_state, "vita40_green_standby") == 0)
    {
        if(counter % 10 == 0)
        {
            return(urgb_u32(vita40_green_standby));
        }
        else
        {
            return(urgb_u32(vita40_off));
        }
    }

    if(strcmp(vita40_state, "vita40_green_steady") == 0)
    {
        return(urgb_u32(vita40_green_steady));
    }

    if(strcmp(vita40_state, "vita40_green_slow") == 0)
    {
        if((counter / 5) % 2 == 0)
        {
            return(urgb_u32(vita40_green_slow));
        }
        else
        {
            return(urgb_u32(vita40_off));
        }
    }

    return(urgb_u32(vita40_off));
}

bool draw_gen_leds(struct repeating_timer *t)
{
    uint32_t    led_values[FRONT_PANEL_LED_COUNT];
    uint16_t    counter = 0;

    // Update LED values
    led_values[0] = vita40_to_urgb(snon_get_value("=W01=WBA03=PFA01"), led_update_counter);
    led_values[1] = vita40_to_urgb(snon_get_value("=W01=WBA02=PFA01"), led_update_counter);
    led_values[2] = vita40_to_urgb(snon_get_value("=W01=WBA01=PFA01"), led_update_counter);

    led_values[3] = vita40_to_urgb(snon_get_value("=W01=WBA01=PFA02"), led_update_counter);
    led_values[4] = vita40_to_urgb(snon_get_value("=W01=WBA02=PFA02"), led_update_counter);
    led_values[5] = vita40_to_urgb(snon_get_value("=W01=WBA03=PFA02"), led_update_counter);

    led_values[6] = vita40_to_urgb(snon_get_value("=W01=WBA01=PFA03"), led_update_counter);
    led_values[7] = vita40_to_urgb(snon_get_value("=W01=WBA02=PFA03"), led_update_counter);
    led_values[8] = vita40_to_urgb(snon_get_value("=W01=WBA03=PFA03"), led_update_counter);

    led_values[9] = vita40_to_urgb(snon_get_value("=Q21=PFA01"), led_update_counter);
    led_values[10] = vita40_to_urgb(snon_get_value("=Q21=QBA01=PFA01"), led_update_counter);
    led_values[11] = vita40_to_urgb(snon_get_value("=Q21=QBA02=PFA01"), led_update_counter);
    led_values[12] = vita40_to_urgb(snon_get_value("=Q21=QBA03=PFA01"), led_update_counter);

    led_values[13] = vita40_to_urgb(snon_get_value("=W02=WBA01=PGC01=PFA01"), led_update_counter);
    led_values[14] = vita40_to_urgb(snon_get_value("=W02=WBA02=PGC01=PFA01"), led_update_counter);
    led_values[15] = vita40_to_urgb(snon_get_value("=W02=WBA03=PGC01=PFA01"), led_update_counter);

    led_values[16] = vita40_to_urgb(snon_get_value("=W02=WBA01=PGA01=PFA01"), led_update_counter);
    led_values[17] = vita40_to_urgb(snon_get_value("=W02=WBA02=PGA01=PFA01"), led_update_counter);
    led_values[18] = vita40_to_urgb(snon_get_value("=W02=WBA03=PGA01=PFA01"), led_update_counter);

    led_values[19] = vita40_to_urgb(snon_get_value("=W02=WBA01=PFA01"), led_update_counter);
    led_values[20] = vita40_to_urgb(snon_get_value("=W02=WBA02=PFA01"), led_update_counter);
    led_values[21] = vita40_to_urgb(snon_get_value("=W02=WBA03=PFA01"), led_update_counter);

    led_values[22] = vita40_to_urgb(snon_get_value("=Q20=PFA01"), led_update_counter);
    led_values[23] = vita40_to_urgb(snon_get_value("=Q20=QBA01=PFA01"), led_update_counter);
    led_values[24] = vita40_to_urgb(snon_get_value("=Q20=QBA02=PFA01"), led_update_counter);
    led_values[25] = vita40_to_urgb(snon_get_value("=Q20=QBA03=PFA01"), led_update_counter);

    led_values[26] = vita40_to_urgb(snon_get_value("=W03=WBA01=PFA01"), led_update_counter);
    led_values[27] = vita40_to_urgb(snon_get_value("=W03=WBA02=PFA01"), led_update_counter);
    led_values[28] = vita40_to_urgb(snon_get_value("=W03=WBA03=PFA01"), led_update_counter);

    led_values[29] = vita40_to_urgb(snon_get_value("=W03=WBA01=PGA01=PFA01"), led_update_counter);
    led_values[30] = vita40_to_urgb(snon_get_value("=W03=WBA02=PGA01=PFA01"), led_update_counter);
    led_values[31] = vita40_to_urgb(snon_get_value("=W03=WBA03=PGA01=PFA01"), led_update_counter);

    led_values[32] = vita40_to_urgb(snon_get_value("=W03=WBA01=PGC01=PFA01"), led_update_counter);
    led_values[33] = vita40_to_urgb(snon_get_value("=W03=WBA02=PGC01=PFA01"), led_update_counter);
    led_values[34] = vita40_to_urgb(snon_get_value("=W03=WBA03=PGC01=PFA01"), led_update_counter);

    led_values[35] = vita40_to_urgb(snon_get_value("=Q22=PFA01"), led_update_counter);
    led_values[36] = vita40_to_urgb(snon_get_value("=Q22=QBA01=PFA01"), led_update_counter);
    led_values[37] = vita40_to_urgb(snon_get_value("=Q22=QBA02=PFA01"), led_update_counter);
    led_values[38] = vita40_to_urgb(snon_get_value("=Q22=QBA03=PFA01"), led_update_counter);

    led_values[39] = vita40_to_urgb(snon_get_value("=W04=WBA01=PFA01"), led_update_counter);
    led_values[40] = vita40_to_urgb(snon_get_value("=W04=WBA02=PFA01"), led_update_counter);
    led_values[41] = vita40_to_urgb(snon_get_value("=W04=WBA03=PFA01"), led_update_counter);

    led_values[42] = vita40_to_urgb("vita40_off", led_update_counter);

    led_values[43] = vita40_to_urgb(snon_get_value("=T01=TAA01=PFA01"), led_update_counter);
    led_values[44] = vita40_to_urgb(snon_get_value("=T01=TAA01=PFA02"), led_update_counter);
    led_values[45] = vita40_to_urgb(snon_get_value("=T01=TAA01=PFA03"), led_update_counter);

    led_values[46] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[47] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[48] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[49] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[50] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[51] = vita40_to_urgb("vita40_off", led_update_counter);

    led_values[52] = vita40_to_urgb(snon_get_value("=W05=WBA01=PFA01"), led_update_counter);
    led_values[53] = vita40_to_urgb(snon_get_value("=W05=WBA02=PFA01"), led_update_counter);
    led_values[54] = vita40_to_urgb(snon_get_value("=W05=WBA03=PFA01"), led_update_counter);

    led_values[55] = vita40_to_urgb(snon_get_value("=Q12=PFA01"), led_update_counter);
    led_values[56] = vita40_to_urgb(snon_get_value("=Q12=QBA01=PFA01"), led_update_counter);
    led_values[57] = vita40_to_urgb(snon_get_value("=Q12=QBA02=PFA01"), led_update_counter);
    led_values[58] = vita40_to_urgb(snon_get_value("=Q12=QBA03=PFA01"), led_update_counter);

    led_values[59] = vita40_to_urgb(snon_get_value("=W06=WBA01=PFA01"), led_update_counter);
    led_values[60] = vita40_to_urgb(snon_get_value("=W06=WBA02=PFA01"), led_update_counter);
    led_values[61] = vita40_to_urgb(snon_get_value("=W06=WBA03=PFA01"), led_update_counter);

    led_values[62] = vita40_to_urgb(snon_get_value("=W06=WBA01=PGA01=PFA01"), led_update_counter);
    led_values[63] = vita40_to_urgb(snon_get_value("=W06=WBA02=PGA01=PFA01"), led_update_counter);
    led_values[64] = vita40_to_urgb(snon_get_value("=W06=WBA03=PGA01=PFA01"), led_update_counter);

    led_values[65] = vita40_to_urgb(snon_get_value("=W06=WBA01=PGC01=PFA01"), led_update_counter);
    led_values[66] = vita40_to_urgb(snon_get_value("=W06=WBA02=PGC01=PFA01"), led_update_counter);
    led_values[67] = vita40_to_urgb(snon_get_value("=W06=WBA03=PGC01=PFA01"), led_update_counter);

    led_values[68] = vita40_to_urgb(snon_get_value("=Q10=PFA01"), led_update_counter);
    led_values[69] = vita40_to_urgb(snon_get_value("=Q10=QBA01=PFA01"), led_update_counter);
    led_values[70] = vita40_to_urgb(snon_get_value("=Q10=QBA02=PFA01"), led_update_counter);
    led_values[71] = vita40_to_urgb(snon_get_value("=Q10=QBA03=PFA01"), led_update_counter);

    led_values[72] = vita40_to_urgb(snon_get_value("=W07=WBA01=PFA01"), led_update_counter);
    led_values[73] = vita40_to_urgb(snon_get_value("=W07=WBA02=PFA01"), led_update_counter);
    led_values[74] = vita40_to_urgb(snon_get_value("=W07=WBA03=PFA01"), led_update_counter);

    led_values[75] = vita40_to_urgb(snon_get_value("=Q11=PFA01"), led_update_counter);
    led_values[76] = vita40_to_urgb(snon_get_value("=Q11=QBA01=PFA01"), led_update_counter);
    led_values[77] = vita40_to_urgb(snon_get_value("=Q11=QBA02=PFA01"), led_update_counter);
    led_values[78] = vita40_to_urgb(snon_get_value("=Q11=QBA03=PFA01"), led_update_counter);

    led_values[79] = vita40_to_urgb(snon_get_value("=W08=WBA01=PGC01=PFA01"), led_update_counter);
    led_values[80] = vita40_to_urgb(snon_get_value("=W08=WBA02=PGC01=PFA01"), led_update_counter);
    led_values[81] = vita40_to_urgb(snon_get_value("=W08=WBA03=PGC01=PFA01"), led_update_counter);

    led_values[82] = vita40_to_urgb(snon_get_value("=W08=WBA01=PGA01=PFA01"), led_update_counter);
    led_values[83] = vita40_to_urgb(snon_get_value("=W08=WBA02=PGA01=PFA01"), led_update_counter);
    led_values[84] = vita40_to_urgb(snon_get_value("=W08=WBA03=PGA01=PFA01"), led_update_counter);

    led_values[85] = vita40_to_urgb(snon_get_value("=W08=WBA01=PFA01"), led_update_counter);
    led_values[86] = vita40_to_urgb(snon_get_value("=W08=WBA02=PFA01"), led_update_counter);
    led_values[87] = vita40_to_urgb(snon_get_value("=W08=WBA03=PFA01"), led_update_counter);

    led_values[88] = vita40_to_urgb(snon_get_value("=G01=GAA01=RBA01=PFA01"), led_update_counter);

    led_values[89] = vita40_to_urgb(snon_get_value("=G01=GAA01=PFA01"), led_update_counter);
    led_values[90] = vita40_to_urgb(snon_get_value("=G01=GAA01=PFA02"), led_update_counter);
    led_values[91] = vita40_to_urgb(snon_get_value("=G01=GAA01=PFA03"), led_update_counter);

    led_values[92] = vita40_to_urgb(snon_get_value("=W09=WBA01=PFA01"), led_update_counter);
    led_values[93] = vita40_to_urgb(snon_get_value("=W09=WBA02=PFA01"), led_update_counter);
    led_values[94] = vita40_to_urgb(snon_get_value("=W09=WBA03=PFA01"), led_update_counter);

    led_values[95] = vita40_to_urgb(snon_get_value("=W09=WBA01=PGA01=PFA01"), led_update_counter);
    led_values[96] = vita40_to_urgb(snon_get_value("=W09=WBA02=PGA01=PFA01"), led_update_counter);
    led_values[97] = vita40_to_urgb(snon_get_value("=W09=WBA03=PGA01=PFA01"), led_update_counter);

    led_values[98] = vita40_to_urgb(snon_get_value("=W09=WBA01=PGC01=PFA01"), led_update_counter);
    led_values[99] = vita40_to_urgb(snon_get_value("=W09=WBA02=PGC01=PFA01"), led_update_counter);
    led_values[100] = vita40_to_urgb(snon_get_value("=W09=WBA03=PGC01=PFA01"), led_update_counter);

    led_values[101] = vita40_to_urgb("vita40_off", led_update_counter);
    led_values[102] = vita40_to_urgb("vita40_off", led_update_counter);

    led_values[103] = vita40_to_urgb(snon_get_value("=G01=T02=TAA01=PFA01"), led_update_counter);
    led_values[104] = vita40_to_urgb(snon_get_value("=G01=T02=TAA01=RAC01=PFA01"), led_update_counter);

    led_values[105] = vita40_to_urgb("vita40_off", led_update_counter);

    led_values[106] = vita40_to_urgb(snon_get_value("=W10=WEA01=PFA01"), led_update_counter);
    led_values[107] = vita40_to_urgb(snon_get_value("=G01=T02=TAA01=PGC01=PFA01"), led_update_counter);

    ws2812_program_init(pio1, pio_sm, pio_sm_offset, 18, 800000, false);

    while(counter < FRONT_PANEL_LED_COUNT)
    {
        put_pixel(led_values[counter]);

        counter = counter + 1;
    }

    busy_wait_us(200);

    led_update_counter = led_update_counter + 1;
}