// ---------------------------------------------------------------------------------
// 1840A Substation Panel End Display
// ---------------------------------------------------------------------------------
// HMI output for two LCDs plus front panel LEDs
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// System Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Pico Headers
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/i2c.h"

// Local Headers
#include "pins.h"
#include "pico-utils/ws2812.h"
#include "uart.h"
#include "st7789_lcd.h"
#include "fonts.h"
#include "gt20l16.h"
#include "snon/sha1.h"
#include "snon/snon_utils.h"

#include "images.h"
#include "asm_hmi.h"

// Local Constants
#define DEBUG_WS2812            11
#define SNPRINTF_BUFFER_SIZE    80

// =================================================================================
// Local Functions

void draw_gen_top(void)
{
    st7789_start_pixels(PIN_CS);

    // Title area
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_draw_string_centred(snon_get_value("Bus Designator"), B612_BMA_32, 0, SCREEN_WIDTH, 190);

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



int main() {
    datetime_t  t;
    bool        get_time_valid = false;
    char        snprintf_buffer[SNPRINTF_BUFFER_SIZE];
    uint16_t    counter = 0;
    char*       json_output = NULL;
    bool        refresh_needed = false;

    stdio_init_all();

    printf("\n\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Protonema 1840 Front Panel End Module, Firmware 0.1.0-alpha.3\n");
    printf("CERN-OHL-S v2 (https://github.com/dslik/power-sim/blob/main/license.md)\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Enable debug LED...\n");

    // Get first free state machine in PIO 1
    uint display_sm = pio_claim_unused_sm(pio1, true);
    uint display_sm_offset = pio_add_program(pio1, &ws2812_program);

    // Set the debug LED to purple
    ws2812_program_init(pio1, display_sm, display_sm_offset, DEBUG_WS2812, 1200000, false);
    put_pixel(urgb_u32(10, 0, 10));

    // Wait for the PIO to finish writing out data
    sleep_ms(1);

    // ===========================================================================================
    printf("Testing SHA-1: ");

    SHA1_CTX   sha1_context;
    uint8_t    sha1_result[20];

    SHA1Init(&sha1_context);
    SHA1Update(&sha1_context, "abc", 3);
    SHA1Final(&sha1_result, &sha1_context);

    while(counter != 20)
    {
        printf("%02X", sha1_result[counter]);

        counter = counter + 1;
    }
    printf("\n");

    snon_name_to_eid("device", snprintf_buffer);
    printf("Device eID:   %s\n", snprintf_buffer);


    // ===========================================================================================
    ws2812_program_init(pio1, display_sm, display_sm_offset, 18, 800000, false);
    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));

    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));

    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));
    put_pixel(urgb_u32(10, 0, 0));

    put_pixel(urgb_u32(0, 10, 0));
    put_pixel(urgb_u32(0, 10, 0));
    put_pixel(urgb_u32(0, 10, 0));
    put_pixel(urgb_u32(0, 10, 0));

    put_pixel(urgb_u32(0, 10, 0));
    put_pixel(urgb_u32(0, 10, 0));
    put_pixel(urgb_u32(0, 10, 0));

    // ===========================================================================================
    printf("Initializing SNON entities...\n");

    snon_initialize("1840A Edge Display");

    // Add measurands
    snon_register("Time Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"iso8601\",\"meAq\":\"count\"}");
    snon_register("Seconds Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"numeric\",\"meAq\":\"count\",\"meUS\":{\"*\":\"s\"},\"meUSx\":{\"*\":\"seconds\"},\"meR\":\"1\",\"meAc\":\"1\"}");

    // Add sensors
    snon_register("Device Time Sensor", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("Device Time Sensor", SNON_REL_CHILD_OF, "Device");
    snon_add_relationship("Device Time Sensor", SNON_REL_MEASURAND, "Time Measurand");

    snon_register("Device Uptime Sensor", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("Device Uptime Sensor", SNON_REL_CHILD_OF, "Device");
    snon_add_relationship("Device Uptime Sensor", SNON_REL_MEASURAND, "Seconds Measurand");

    // Add series
    snon_register("Device Time Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("Device Time Series", SNON_REL_MEASURAND, "Time Measurand");
    snon_add_relationship("Device Time Series", SNON_REL_CHILD_OF, "Device Time Sensor");
    snon_add_relationship("Device Time Series", SNON_REL_VALUES, "Device Time");

    snon_register("Device Uptime Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("Device Uptime Series", SNON_REL_MEASURAND, "Seconds Measurand");
    snon_add_relationship("Device Uptime Series", SNON_REL_CHILD_OF, "Device Uptime Sensor");
    snon_add_relationship("Device Uptime Series", SNON_REL_VALUES, "Device Uptime");

    // Add special values
    snon_register("Device Time", SNON_CLASS_VALUE, NULL);
    snon_register("Device Uptime", SNON_CLASS_VALUE, NULL);

    // Add values 
    snon_register("Debug LED RGB", SNON_CLASS_VALUE, NULL);
    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "[\"0A000A\"]");
    snon_set_values("Debug LED RGB", snprintf_buffer);

    snon_register("Bus Designator", SNON_CLASS_VALUE, NULL);
    snon_set_values("Bus Designator", "[\"WAITING\"]");

    snon_register("L1 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current LoLo", "[\"-2\"]");

    snon_register("L1 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current Lo", "[\"-1\"]");

    snon_register("L1 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current", "[\"0\"]");

    snon_register("L1 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current SP", "[\"0\"]");

    snon_register("L1 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current Hi", "[\"1\"]");

    snon_register("L1 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current HiHi", "[\"2\"]");

    snon_register("L2 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current LoLo", "[\"-2\"]");

    snon_register("L2 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current Lo", "[\"-1\"]");

    snon_register("L2 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current", "[\"0\"]");

    snon_register("L2 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current SP", "[\"0\"]");

    snon_register("L2 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current Hi", "[\"1\"]");

    snon_register("L2 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current HiHi", "[\"2\"]");

    snon_register("L3 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current LoLo", "[\"-2\"]");

    snon_register("L3 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current Lo", "[\"-1\"]");

    snon_register("L3 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current", "[\"0\"]");

    snon_register("L3 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current SP", "[\"0\"]");

    snon_register("L3 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current Hi", "[\"1\"]");

    snon_register("L3 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current HiHi", "[\"2\"]");


    // ===========================================================================================
    printf("LCD init...\n");

    // Turn off backlight
    gpio_init(PIN_BL);
    gpio_set_dir(PIN_BL, GPIO_OUT);
    gpio_put(PIN_BL, 0);

    // Initialize the LCD
    st7789_init();
    st7789_init_lcd(PIN_CS);
    st7789_init_lcd(PIN_CS_2);

    // Clear the top LCD
    st7789_start_pixels(PIN_CS);
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));
    st7789_draw_rect(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
    st7789_end_pixels();

    // Turn on backlight
    gpio_put(PIN_BL, 1);

    printf("LCD initialized...\n");

    draw_gen_top();

    printf("Displaying characters from font chip...\n");
    gt20l16_init();

    st7789_start_pixels(PIN_CS_2);

    gt20l16_draw_string("Hello World!", 0, 224, 1);
    gt20l16_draw_string("Actual size of the font.", 0, 208, 1);

    gt20l16_draw_string("Hello World!", 0, 84, 2);
    gt20l16_draw_string("Doubled font.", 0, 68, 2);

    st7789_end_pixels();
    st7789_start_pixels(PIN_CS_2);

    st7789_set_fgcolor(0xFFFF);
    st7789_set_bgcolor(0x0000);

    gt20l16_draw_string("Hello", 0, 16, 4);
    gt20l16_draw_string("World!", 0, 0, 4);

    st7789_draw_image(image2, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_draw_string_centred("ABCDEFGHIJK", B612_BMA_32, 0, SCREEN_WIDTH, 190);
    st7789_draw_string_centred("L1", B612_BMA_24, 0, 80, 160);
    st7789_draw_string_centred("440", B612_BMA_24, 0, 80, 90);
    st7789_draw_string_centred("A", B612_BMA_24, 0, 80, 60);

    st7789_end_pixels();

    // ===========================================================================================
    printf("Initializing Serial I/O...\n");
    uart_setup();

    printf("Ready for commands\n");
    while (true)
    {
        // Check if there are any commands pending
        const char* command = uart_command_get();
        if(strcmp(command, "") != 0)
        {
            if(strcmp(command, "help") == 0)
            {
                uart_puts(uart1, "\r\nCommands:");
                uart_puts(uart1, "\r\n\"help\"                - Displays list of commands");
                uart_puts(uart1, "\r\n\"clear\"               - Clear the serial terminal");
                uart_puts(uart1, "\r\n\"ls\"                  - List SNON entites");
                uart_puts(uart1, "\r\n\"cat <entity>\"        - Display the value of an SNON entity");
                uart_puts(uart1, "\r\n\"get time\"            - Get the current time");
                uart_puts(uart1, "\r\n\"set time\"            - Set the current time");
                uart_command_clear();
            }
            else if(strcmp(command, "clear") == 0)
            {
                uart_puts(uart1, "\033[2J");
                uart_command_clear();
            }
            else if(strcmp(command, "ls") == 0)
            {
                // List all entities
                char*       entity_name = NULL;
                uint16_t    counter = 0;
                uint16_t    json_output_length = 0;

                json_output = snon_get_values("Entities");

                if(json_output != NULL)
                {
                    json_output_length = strlen(json_output);

                    counter = 0;
                    uart_puts(uart1, "\r\n");

                    while(json_output[counter] != 0)
                    {
                        if(json_output[counter] == '"')
                        {
                            sscanf(&json_output[counter + 1], "%45s", &snprintf_buffer);

                            uart_puts(uart1, snprintf_buffer);
                            uart_puts(uart1, " - ");

                            entity_name = snon_get_name(snprintf_buffer);

                            if(entity_name != NULL)
                            {
                                uart_puts(uart1, entity_name);
                            }
                            else
                            {
                                uart_puts(uart1, "Unknown entity name");
                            }
                                
                            if(counter + 50 < json_output_length)
                            {
                                uart_puts(uart1, "\r\n");
                            }

                            counter = counter + SNON_URN_LENGTH;
                        }

                        counter = counter + 1;
                    }

                    uart_puts(uart1, "\r\n");
                    free(json_output);
                }

                uart_command_clear();
            }
            else if(command[0] == '{')
            {
                char*       json_output = NULL;
                char        eid[SNON_URN_LENGTH];

                if(command[1] == '}')
                {
                    // Display all entities
                    char*       entity_output = NULL;
                    uint16_t    counter = 0;
                    uint16_t    json_output_length = 0;

                    json_output = snon_get_values("Entities");

                    if(json_output != NULL)
                    {
                        json_output_length = strlen(json_output);

                        counter = 0;
                        uart_puts(uart1, "\r\n[");

                        while(json_output[counter] != 0)
                        {
                            if(json_output[counter] == '"')
                            {
                                sscanf(&json_output[counter + 1], "%45s", &snprintf_buffer);
                                entity_output = snon_get_json(snprintf_buffer);

                                if(entity_output != NULL)
                                {
                                    uart_puts(uart1, entity_output);
                                    free(entity_output);
                                }
                                else
                                {
                                    uart_puts(uart1, "{}");
                                }
                                
                                if(counter + 50 < json_output_length)
                                {
                                    uart_puts(uart1, ",");
                                }

                                counter = counter + SNON_URN_LENGTH;
                            }

                            counter = counter + 1;
                        }

                        uart_puts(uart1, "]");
                        free(json_output);
                    }
                }
                else
                {
                    if(json_has_eid(command, snprintf_buffer) == true)
                    {
                        // Copy over the eID
                        strncpy(eid, snprintf_buffer, SNON_URN_LENGTH);
                        eid[SNON_URN_LENGTH - 1] = 0;
                        
                        if(json_has_value(command, snprintf_buffer) == true)
                        {
                            // Update the value
                            printf("New Value %s\n", snprintf_buffer);
                            snon_set_values(eid, snprintf_buffer);
                            refresh_needed = true;
                        }

                        json_output = snon_get_json(eid);

                        if(json_output != NULL)
                        {
                            uart_puts(uart1, "\r\n");
                            uart_puts(uart1, json_output);
                            free(json_output);
                        }
                        else
                        {
                            printf("Unable to find entity with eID %s\r\n", eid);
                            uart_puts(uart1, "\r\n{\r\n}");
                        }
                    }
                    else
                    {
                        printf("Unable to find eID\r\n");
                        uart_puts(uart1, "\r\n{\r\n}");
                    }
                }

                uart_command_clear();
            }
            else if(strncmp(command, "cat ", 4) == 0)
            {
                char* json_output = snon_get_json((char*) &(command[4]));

                if(json_output != NULL)
                {
                    uart_puts(uart1, "\r\n");

                    counter = 0;
                    while(json_output[counter] != 0)
                    {
                        uart_putc(uart1, json_output[counter]);

                        if(json_output[counter] == ',')
                        {
                            uart_putc(uart1, '\r');
                            uart_putc(uart1, '\n');
                        }

                        counter = counter + 1;
                    }

                    uart_puts(uart1, "\r\n");
                    free(json_output);
                }
                else
                {
                    uart_puts(uart1, "\r\nEntity not found\r\n");
                }

                uart_command_clear();
            }
            else if(strncmp(command, "dump", 4) == 0)
            {
                char* json_output = snon_get_dump();

                if(json_output != NULL)
                {
                    uart_puts(uart1, "\r\n");

                    counter = 0;
                    while(json_output[counter] != 0)
                    {
                        if(json_output[counter] == '\n')
                        {
                            uart_putc(uart1, '\r');
                        }

                        uart_putc(uart1, json_output[counter]);

                        counter = counter + 1;
                    }

                    uart_puts(uart1, "\r\n");
                    free(json_output);
                }
                else
                {
                    uart_puts(uart1, "\r\nDump Error\r\n");
                }

                uart_command_clear();
            }
            else if(strcmp(command, "get time") == 0)
            {
                get_time_valid = rtc_counter_to_iso8601(snprintf_buffer, time_us_64());

                if(get_time_valid == true)
                {
                    uart_puts(uart1, "\r\nThe time is: ");
                    uart_puts(uart1, snprintf_buffer);
                }
                else
                {
                    uart_puts(uart1, "\r\nRTC not running. Use the \"set time\" command to set the current time.");
                }
                uart_command_clear();
            }
            else if(strncmp(command, "set time ", 9) == 0)
            {
                unsigned int    year = 0;
                unsigned int    month = 0;
                unsigned int    day = 0;
                unsigned int    hours = 0;
                unsigned int    minutes = 0;
                unsigned int    seconds = 0;
                bool            set_valid = false;

                sscanf(command, "set time %4u-%2u-%2uT%2u:%2u:%2uZ", &year, &month, &day, &hours, &minutes, &seconds);
                if(year < 2022 || year > 2055)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid year %04d. Must be between 2022 and 2055", year);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(month < 1 || month > 12)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid month %02d. Must be between 1 and 12", month);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(day < 1 || day > 31)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid day %02d. Must be between 1 and 31", day);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(hours < 0 || hours > 23)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid hour %02d. Must be between 0 and 23", hours);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(minutes < 0 || minutes > 59)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid minute %02d. Must be between 0 and 59", minutes);
                    uart_puts(uart1, snprintf_buffer);
                }
                else if(seconds < 0 || seconds > 59)
                {
                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nInvalid second %02d. Must be between 0 and 59", hours);
                    uart_puts(uart1, snprintf_buffer);
                }
                else
                {
                    t.year = year;
                    t.month = month;
                    t.day = day;
                    t.hour = hours;
                    t.min = minutes;
                    t.sec = seconds;
                    t.dotw = 0;

                    snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "%04d-%02d-%02dT%02d:%02d:%02dZ", t.year, t.month, t.day, t.hour, t.min, t.sec);

                    set_valid = rtc_set_time(snprintf_buffer);
                    sleep_us(64);

                    if(set_valid == true)
                    {
                        uart_puts(uart1, "\r\nTime set to ");
                        uart_puts(uart1, snprintf_buffer);
                    }
                    else
                    {
                        uart_puts(uart1, "\r\nError setting time to ");
                        uart_puts(uart1, snprintf_buffer);
                    }
                }

                uart_command_clear();
            }
            else
            {
                uart_puts(uart1, "\r\nUnknown command\r\n");
                uart_command_clear();
            }
        }

        if(refresh_needed == true)
        {
            draw_gen_top();

            json_output = snon_get_values("Debug LED RGB");

            if(json_output)
            {
                uint8_t r_value = 0;
                uint8_t g_value = 0;
                uint8_t b_value = 0;

                sscanf(json_output, "[\"%2X%2X%2X\"]", &r_value, &g_value, &b_value);
                ws2812_program_init(pio1, display_sm, display_sm_offset, DEBUG_WS2812, 1200000, false);
                put_pixel(urgb_u32(r_value, g_value, b_value));
                free(json_output);
            }

            refresh_needed = false;
        }

        sleep_ms(100);

    }
}
