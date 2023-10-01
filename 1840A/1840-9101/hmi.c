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

// Private prototypes
uint32_t vita40_to_urgb(char* vita40_state, uint32_t counter);

// Global variables
uint32_t        led_update_counter = 0;
extern uint     pio_sm;
extern uint     pio_sm_offset;

// Functions
void draw_gen_top(void)
{
    st7789_start_pixels(PIN_CS);

    // Title area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred(snon_get_value("=W01=PHA01"), B612_BMA_32, 0, SCREEN_WIDTH, 190);

    // Subtitle area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred("L1", B612_BMA_24, (SCREEN_WIDTH / 3) * 0, (SCREEN_WIDTH / 3) * 1, 160);
    st7789_draw_string_centred("L2", B612_BMA_24, (SCREEN_WIDTH / 3) * 1, (SCREEN_WIDTH / 3) * 2, 160);
    st7789_draw_string_centred("L3", B612_BMA_24, (SCREEN_WIDTH / 3) * 2, (SCREEN_WIDTH / 3) * 3, 160);

    // Value indicators
    asm_draw_value_indicator(1, snon_get_value_as_double("L1 Voltage LoLo"),
                                snon_get_value_as_double("L1 Voltage Lo"),
                                snon_get_value_as_double("L1 Voltage"),
                                snon_get_value_as_double("L1 Voltage Hi"),
                                snon_get_value_as_double("L1 Voltage HiHi"),
                                snon_get_value_as_double("L1 Voltage SP"));

    asm_draw_value_indicator(2, snon_get_value_as_double("L2 Voltage LoLo"),
                                snon_get_value_as_double("L2 Voltage Lo"),
                                snon_get_value_as_double("L2 Voltage"),
                                snon_get_value_as_double("L2 Voltage Hi"),
                                snon_get_value_as_double("L2 Voltage HiHi"),
                                snon_get_value_as_double("L2 Voltage SP"));

    asm_draw_value_indicator(3, snon_get_value_as_double("L2 Voltage LoLo"),
                                snon_get_value_as_double("L2 Voltage Lo"),
                                snon_get_value_as_double("L2 Voltage"),
                                snon_get_value_as_double("L2 Voltage Hi"),
                                snon_get_value_as_double("L2 Voltage HiHi"),
                                snon_get_value_as_double("L2 Voltage SP"));

    // Value area
    asm_draw_flow_value(1, snon_get_value_as_double("L1 Voltage"), "V");
    asm_draw_flow_value(2, snon_get_value_as_double("L2 Voltage"), "V");
    asm_draw_flow_value(3, snon_get_value_as_double("L3 Voltage"), "V");

    // Flow area
    asm_draw_flow_arrow(1, asm_flow_none);
    asm_draw_flow_arrow(2, asm_flow_none);
    asm_draw_flow_arrow(3, asm_flow_none);

    //asm_draw_value_alarm(2, asm_alarm_one);
    //asm_draw_value_alarm(3, asm_alarm_two);

    st7789_end_pixels();
}

void draw_gen_bottom(void)
{
    st7789_start_pixels(PIN_CS_2);

    // Title area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred(snon_get_value("=W01=PHA01"), B612_BMA_32, 0, SCREEN_WIDTH, 190);

    // Subtitle area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred("L1", B612_BMA_24, (SCREEN_WIDTH / 3) * 0, (SCREEN_WIDTH / 3) * 1, 160);
    st7789_draw_string_centred("L2", B612_BMA_24, (SCREEN_WIDTH / 3) * 1, (SCREEN_WIDTH / 3) * 2, 160);
    st7789_draw_string_centred("L3", B612_BMA_24, (SCREEN_WIDTH / 3) * 2, (SCREEN_WIDTH / 3) * 3, 160);

    // Value indicators
    asm_draw_value_indicator(1, snon_get_value_as_double("L1 Current LoLo"),
                                snon_get_value_as_double("L1 Current Lo"),
                                snon_get_value_as_double("L1 Current"),
                                snon_get_value_as_double("L1 Current Hi"),
                                snon_get_value_as_double("L1 Current HiHi"),
                                snon_get_value_as_double("L1 Current SP"));

    asm_draw_value_indicator(2, snon_get_value_as_double("L2 Current LoLo"),
                                snon_get_value_as_double("L2 Current Lo"),
                                snon_get_value_as_double("L2 Current"),
                                snon_get_value_as_double("L2 Current Hi"),
                                snon_get_value_as_double("L2 Current HiHi"),
                                snon_get_value_as_double("L2 Current SP"));

    asm_draw_value_indicator(3, snon_get_value_as_double("L2 Current LoLo"),
                                snon_get_value_as_double("L2 Current Lo"),
                                snon_get_value_as_double("L2 Current"),
                                snon_get_value_as_double("L2 Current Hi"),
                                snon_get_value_as_double("L2 Current HiHi"),
                                snon_get_value_as_double("L2 Current SP"));

    // Value area
    asm_draw_flow_value(1, snon_get_value_as_double("L1 Current"), "A");
    asm_draw_flow_value(2, snon_get_value_as_double("L2 Current"), "A");
    asm_draw_flow_value(3, snon_get_value_as_double("L3 Current"), "A");

    // Flow area
    if(snon_get_value_as_double("L1 Current") >=0)
    {
        asm_draw_flow_arrow(1, asm_flow_up);
    }
    else
    {
        asm_draw_flow_arrow(1, asm_flow_down);
    }

    if(snon_get_value_as_double("L2 Current") >=0)
    {
        asm_draw_flow_arrow(2, asm_flow_up);
    }
    else
    {
        asm_draw_flow_arrow(2, asm_flow_down);
    }

    if(snon_get_value_as_double("L3 Current") >=0)
    {
        asm_draw_flow_arrow(3, asm_flow_up);
    }
    else
    {
        asm_draw_flow_arrow(3, asm_flow_down);
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
    //if(pio_cs == false)
    {
        ws2812_program_init(pio1, pio_sm, pio_sm_offset, 18, 800000, false);
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA03=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA01=PFA02"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA02=PFA02"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA03=PFA02"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA01=PFA03"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA02=PFA03"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W01=WBA03=PFA03"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q21=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q21=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q21=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q21=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA01=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA02=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA03=PGC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA01=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA02=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA03=PGA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W02=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q20=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q20=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q20=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q20=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA01=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA02=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA03=PGA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA01=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA02=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W03=WBA03=PGC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q22=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q22=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q22=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q22=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W04=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W04=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W04=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=T01=TAA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=T01=TAA01=PFA02"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=T01=TAA01=PFA03"), led_update_counter));

        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W05=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W05=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W05=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q12=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q12=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q12=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q12=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA01=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA02=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA03=PGA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA01=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA02=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W06=WBA03=PGC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q10=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q10=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q10=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q10=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W07=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W07=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W07=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=Q11=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q11=QBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q11=QBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=Q11=QBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA01=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA02=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA03=PGC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA01=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA02=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA03=PGA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W08=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=G01=GAA01=RBA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=G01=GAA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=G01=GAA01=PFA02"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=G01=GAA01=PFA03"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA02=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA03=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA01=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA02=PGA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA03=PGA01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA01=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA02=PGC01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=W09=WBA03=PGC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));
        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=G01=T02=TAA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=G01=T02=TAA01=RAC01=PFA01"), led_update_counter));

        put_pixel(vita40_to_urgb("vita40_off", led_update_counter));

        put_pixel(vita40_to_urgb(snon_get_value("=W10=WEA01=PFA01"), led_update_counter));
        put_pixel(vita40_to_urgb(snon_get_value("=G01=T02=TAA01=PGC01=PFA01"), led_update_counter));

        busy_wait_us(200);

        led_update_counter = led_update_counter + 1;
    }
}