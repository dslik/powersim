// ---------------------------------------------------------------------------------
// 1841A Substation Panel Centre Display
// ---------------------------------------------------------------------------------
// HMI output for three LCDs plus front panel buttons
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
#include "pico/multicore.h"
#include "hardware/rtc.h"
#include "hardware/i2c.h"

// Local Headers
#include "pins.h"
#include "pico-utils/ws2812.h"
#include "uart.h"
#include "build.h"
#include "sensors.h"
#include "hmi.h"
#include "st7789_lcd.h"
#include "gt20l16.h"
#include "mcp23017.h"
#include "snon/sha1.h"
#include "snon/snon_utils.h"
#include "mem_utils.h"

#include "asm_hmi.h"
#include "vita40.h"

// Local Constants
#define SNPRINTF_BUFFER_SIZE    80

// Global Variables
uint            pio_sm = 0;
uint            pio_sm_offset = 0;
bool            refresh_needed = false;

// =================================================================================
// Local Functions

void hmi_main(void)
{
    // Turn off backlight
    gpio_init(PIN_BL);
    gpio_set_dir(PIN_BL, GPIO_OUT);
    gpio_put(PIN_BL, 0);

    // Initialize the LCD
    st7789_init();
    st7789_init_lcd(PIN_CS_1);
    st7789_init_lcd(PIN_CS_2);
    st7789_init_lcd(PIN_CS_3);

    // Clear the left LCD
    st7789_start_pixels(PIN_CS_1);
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));
    st7789_draw_rect(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
    st7789_end_pixels();

    // Clear the middle LCD
    st7789_start_pixels(PIN_CS_2);
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));
    st7789_draw_rect(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
    st7789_end_pixels();

    // Clear the right LCD
    st7789_start_pixels(PIN_CS_3);
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));
    st7789_draw_rect(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
    st7789_end_pixels();

    // Turn on backlight
    gpio_put(PIN_BL, 1);

    init_gen_screens();

    while(true)
    {
        //sleep_ms(1000);
        update_gen_screens();
    }
}


int main() {
    struct  repeating_timer ledTimer;
    datetime_t  t;
    bool        get_time_valid = false;
    char        snprintf_buffer[SNPRINTF_BUFFER_SIZE];
    uint16_t    counter = 0;
    char*       json_output = NULL;
    int         ret = PICO_ERROR_NONE;

    stdio_init_all();

    printf("\n\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Protonema 1841 Front Panel End Module, Firmware %s\n", FW_VERSION);
    printf("CERN-OHL-S v2 (https://github.com/dslik/power-sim/blob/main/license.md)\n");
    printf("---------------------------------------------------------------------------------\n");
    printf("Enable debug LED...\n");

    // Get first free state machine in PIO 1
    pio_sm = pio_claim_unused_sm(pio1, true);
    pio_sm_offset = pio_add_program(pio1, &ws2812_program);

    // Set the debug LED to purple
    ws2812_program_init(pio1, pio_sm, pio_sm_offset, DEBUG_WS2812, 1200000, false);
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
    printf("Device eID:    %s\n", snprintf_buffer);

    // ===========================================================================================
    printf("Initializing SNON entities (%lu)\n", get_free_ram_2());
    sensors_initialize();
    printf("SNON entities initialized. (%lu)\n", get_free_ram_2());

    // ===========================================================================================
    printf("Front Panel init...\n");
    multicore_launch_core1(&hmi_main);

    add_repeating_timer_ms(100, draw_gen_leds, NULL, &ledTimer);

    printf("LCD initialized...\n");

    // ===========================================================================================
    printf("Initializing Serial I/O...\n");
    uart_setup();

    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    i2c_init(i2c1, 100 * 1000);


    // ===========================================================================================
    printf("Initializing Buttons...\n");

    // Set all GPIO A I/O lines to be outputs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_2, 0x00, 0b00000000);
    // Set all GPIO A I/O lines to zero
    ret = mcp23017_write_register(i2c1, I2C_ADDR_2, 0x12, 0b00000000);

    // Set all GPIO B I/O lines to be inputs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_2, 0x01, 0b11111111);
    // Enable all GPIO B I/O line pull-up Resistors
    ret = mcp23017_write_register(i2c1, I2C_ADDR_2, 0x0D, 0b11111111);

    // Turn on all LEDs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_2, 0x12, 0b11111111);

    // Set all GPIO A I/O lines to be outputs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_3, 0x00, 0b00000000);
    // Set all GPIO A I/O lines to zero
    ret = mcp23017_write_register(i2c1, I2C_ADDR_3, 0x12, 0b00000000);

    // Set all GPIO B I/O lines to be inputs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_3, 0x01, 0b11111111);
    // Enable all GPIO B I/O line pull-up Resistors
    ret = mcp23017_write_register(i2c1, I2C_ADDR_3, 0x0D, 0b11111111);

    // Turn on all LEDs
    ret = mcp23017_write_register(i2c1, I2C_ADDR_3, 0x12, 0b11111111);


    printf("Ready for commands\n");
    while (true)
    {
        // Check if there are any commands pending
        const char* command = uart_command_get();
        if(strcmp(command, "") != 0)
        {
            if(command[0] == '{')
            {
                char*       json_output = NULL;
                char        eid[SNON_URN_LENGTH];

                printf("Received SNON fragment \"%s\"\n", command);

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
                        uart_puts(uart1, "[");

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
                            uart_puts(uart1, json_output);
                            free(json_output);
                        }
                        else
                        {
                            printf("Unable to find entity with eID %s\r\n", eid);
                            uart_puts(uart1, "{}");
                        }
                    }
                    else
                    {
                        printf("Unable to find eID\r\n");
                        uart_puts(uart1, "{}");
                    }
                }

                uart_puts(uart1, "\r\n");
                uart_command_clear();
            }
            else
            {
                if(uart_in_command_mode())
                {
                    printf("Received Command \"%s\"\n", command);

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
                        else
                        {
                            uart_puts(uart1, "\r\nError: Out of memory.\r\n");
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
                            uart_puts(uart1, "\r\nError: Out of memory.\r\n");
                        }

                        uart_command_clear();
                    }
                    else if(strcmp(command, "get mem") == 0)
                    {
                        snprintf(snprintf_buffer, SNPRINTF_BUFFER_SIZE, "\r\nFree memory: %lu bytes\r\n", get_free_ram_2());
                        uart_puts(uart1, snprintf_buffer);

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
                    else if(strcmp(command, "exit") == 0)
                    {
                        uart_command_exit();
                        uart_command_clear();
                    }
                    else
                    {
                        uart_puts(uart1, "\r\nUnknown command\r\n");
                        uart_command_clear();
                    }
                }
            }
        }


        if(refresh_needed == true)
        {
            json_output = snon_get_values("Debug LED RGB");

            if(json_output)
            {
                uint8_t r_value = 0;
                uint8_t g_value = 0;
                uint8_t b_value = 0;

                sscanf(json_output, "[\"%2X%2X%2X\"]", &r_value, &g_value, &b_value);
                ws2812_program_init(pio1, pio_sm, pio_sm_offset, DEBUG_WS2812, 1200000, false);
                put_pixel(urgb_u32(r_value, g_value, b_value));
                free(json_output);
            }

            refresh_needed = false;
        }

        //sleep_ms(1);
    }
}