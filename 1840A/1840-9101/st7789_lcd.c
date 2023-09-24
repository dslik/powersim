// ---------------------------------------------------------------------------------
// ST7789 Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist in programming with ST7789-style LCDs.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/protonema/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "st7789_lcd.h"

#if __has_include("pins.h")
#include "pins.h"
#else
#include "test_pins.h"
#endif

// Global variables
volatile PIO        PIO_global;
volatile uint       sm_global = 0;
volatile uint8_t    cs_pin = 0;
volatile uint16_t   bgcolour = 0;
volatile uint16_t   fgcolour = 0;
volatile bool       bgtransparent = false;
volatile bool       fgtransparent = false;

// Prototypes
extern void lcd_set_dc_cs(uint8_t display_cs_pin, bool dc, bool cs);
extern void lcd_write_cmd(uint8_t display_cs_pin, const uint8_t *cmd, size_t count);

// =================================================================================
// SPDX-SnippetBegin
// SPDX-SnippetCopyrightText:  Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
// SDPX-SnippetAttributionText: https://github.com/raspberrypi/pico-examples/blob/master/pio/st7789_lcd/st7789_lcd.c
// SPDX-License-Identifier: BSD-3-Clause
// ---------------------------------------------------------------------------------
#define SERIAL_CLK_DIV 5.0f

// Format: cmd length (including cmd byte), post delay in units of 5 ms, then cmd payload
// Note the delays have been shortened a little
static const uint8_t st7789_init_seq[] = {
        1, 20, 0x01,                         // Software reset
        1, 10, 0x11,                         // Exit sleep mode
        2, 2, 0x3a, 0x55,                    // Set colour mode to 16 bit
        2, 0, 0x36, 0b1100000,                    // Set MADCTL: row then column, refresh is bottom to top ????
        5, 0, 0x2a, 0x00, 0x00, SCREEN_WIDTH >> 8, SCREEN_WIDTH & 0xff,   // CASET: column addresses
        5, 0, 0x2b, 0x00, 0x00, SCREEN_HEIGHT >> 8, SCREEN_HEIGHT & 0xff, // RASET: row addresses
        1, 2, 0x21,                          // Inversion on, then 10 ms delay (supposedly a hack?)
        1, 2, 0x13,                          // Normal display on, then 10 ms delay
        1, 2, 0x29,                          // Main screen turn on, then wait 500 ms
        0                                    // Terminate list
};

// DS: Added display_cs_pin input to allow support for multiple displays
inline void lcd_set_dc_cs(uint8_t display_cs_pin, bool dc, bool cs)
{
    sleep_us(1);
    gpio_put_masked((1u << PIN_DC) | (1u << display_cs_pin), !!dc << PIN_DC | !!cs << display_cs_pin);
    sleep_us(1);
}

// DS: Added display_cs_pin input to allow support for multiple displays
inline void lcd_write_cmd(uint8_t display_cs_pin, const uint8_t *cmd, size_t count)
{
    st7789_lcd_wait_idle(PIO_global, sm_global);
    lcd_set_dc_cs(display_cs_pin, 0, 0);
    st7789_lcd_put(PIO_global, sm_global, *cmd++);
    if (count >= 2) {
        st7789_lcd_wait_idle(PIO_global, sm_global);
        lcd_set_dc_cs(display_cs_pin, 1, 0);
        for (size_t i = 0; i < count - 1; ++i)
            st7789_lcd_put(PIO_global, sm_global, *cmd++);
    }
    st7789_lcd_wait_idle(PIO_global, sm_global);
    lcd_set_dc_cs(display_cs_pin, 1, 1);
}

// DS: Added display_cs_pin input to allow support for multiple displays
inline void lcd_init(uint8_t display_cs_pin, const uint8_t *init_seq)
{
    const uint8_t *cmd = init_seq;
    while (*cmd) {
        lcd_write_cmd(display_cs_pin, cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
}

// DS: Added display_cs_pin input to allow support for multiple displays
// DS: Added restting of display window
inline void st7789_start_pixels(uint8_t display_cs_pin)
{
    cs_pin = display_cs_pin;

    st7789_set_window(0, SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT - 1);

    uint8_t cmd = 0x2c; // RAMWR
    lcd_write_cmd(cs_pin, &cmd, 1);
    lcd_set_dc_cs(cs_pin, 1, 0);
}

// SPDX-SnippetEnd
// ---------------------------------------------------------------------------------

void st7789_init(void)
{
    PIO_global = pio0;
    sm_global = 0;
    uint offset = pio_add_program(PIO_global, &st7789_lcd_program);

    st7789_lcd_program_init(PIO_global, sm_global, offset, PIN_DIN, PIN_CLK, SERIAL_CLK_DIV);

    gpio_init(PIN_DC);
    gpio_init(PIN_RESET);

    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_RESET, GPIO_OUT);

    st7789_set_bgcolor(0x0000);
    st7789_set_bgcolor(0xFFFF);
}

void st7789_init_lcd(uint8_t display_cs_pin)
{
    gpio_init(display_cs_pin);
    gpio_set_dir(display_cs_pin, GPIO_OUT);

    gpio_put(display_cs_pin, 1);
    gpio_put(PIN_RESET, 1);
    lcd_init(display_cs_pin, st7789_init_seq);
}

inline void st7789_set_pixel(uint16_t colour)
{
    st7789_lcd_put(PIO_global, sm_global, colour >> 8);
    st7789_lcd_put(PIO_global, sm_global, colour & 0xff);
}

inline void st7789_set_window(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    uint8_t x1_high = x1 >> 8;
    uint8_t x1_low = x1 & 0x00FF;

    uint8_t x2_high = x2 >> 8;
    uint8_t x2_low = x2 & 0x00FF;

    uint8_t y1_high = y1 >> 8;
    uint8_t y1_low = y1 & 0x00FF;

    uint8_t y2_high = y2 >> 8;
    uint8_t y2_low = y2 & 0x00FF;

    uint8_t cmd1[5] = {0x2B, x1_high, x1_low, x2_high, x2_low}; // Set Col
    uint8_t cmd2[5] = {0x2A, y1_high, y1_low, y2_high, y2_low}; // Set Row
    uint8_t cmd3[1] = {0x2c}; // RAMWR

    lcd_write_cmd(cs_pin, cmd1, 5);
    lcd_write_cmd(cs_pin, cmd2, 5);
    lcd_write_cmd(cs_pin, cmd3, 1);
    lcd_set_dc_cs(cs_pin, 1, 0);
}

inline void st7789_set_pixel_xy(uint16_t colour, uint16_t x, uint16_t y, uint8_t scale)
{
    uint8_t pixels = scale * scale;

    st7789_set_window(x * scale, (x * scale) + (scale - 1), y * scale, (y * scale) + (scale - 1));

    while(pixels != 0)
    {
        st7789_set_pixel(colour);
        pixels = pixels - 1;
    }
}

inline void st7789_set_halfpixel(uint8_t value)
{
    st7789_lcd_put(PIO_global, sm_global, value);
}

inline void st7789_end_pixels(void)
{
    lcd_set_dc_cs(cs_pin, 1, 1);
}

// Colour management routines
// ---------------------------------------------------------------------------------

inline void st7789_set_bgcolor(uint16_t colour)
{
    bgcolour = colour;
}

inline uint16_t st7789_get_bgcolor(void)
{
    return(bgcolour);
}

inline void st7789_set_fgcolor(uint16_t colour)
{
    fgcolour = colour;
}

inline uint16_t st7789_get_fgcolor()
{
    return(fgcolour);
}

inline uint16_t st7789_rgb_to_colour(uint8_t red, uint8_t green, uint8_t blue)
{
    return(((red / 8) << 11) + ((green / 4) << 5) + (blue / 8));
}

inline uint8_t st7789_colour_to_red(uint16_t colour)
{
    return(((colour & 0b1111100000000000) >> 11) * 8);
}

inline uint8_t st7789_colour_to_green(uint16_t colour)
{
    return(((colour & 0b0000011111100000) >> 5) * 4);
}

inline uint8_t st7789_colour_to_blue(uint16_t colour)
{
    return(((colour & 0b0000000000011111) >> 0) * 8);
}


// Basic drawing routines
// ---------------------------------------------------------------------------------

inline void st7789_draw_rect(uint16_t width, uint16_t height, uint16_t x_offset, uint16_t y_offset, uint16_t thickness)
{
    for (int x = x_offset; x < x_offset + width; ++x) {
        for (int y = y_offset; y < y_offset + height; ++y) {
            if(x < x_offset + thickness ||
               x >= x_offset + width - thickness ||
               y < y_offset + thickness ||
               y >= y_offset + height - thickness)
            {
                if(!fgtransparent)
                {
                    st7789_set_pixel_xy(fgcolour, x, y, 1);
                }
            }
            else
            {
                if(!bgtransparent)
                {
                    st7789_set_pixel_xy(bgcolour, x, y, 1);
                }
            }
        }
    }
}

inline void st7789_draw_triangle(uint16_t width, uint16_t height, uint16_t x_offset, uint16_t y_offset, uint8_t rotation, uint16_t thickness)
{
    float ax = 0;
    float ay = 0;
    float bx = 0;
    float by = 0;
    float cx = 0;
    float cy = 0;
    float iax = 0;
    float iay = 0;
    float ibx = 0;
    float iby = 0;
    float icx = 0;
    float icy = 0;
    float a = 0;
    float b = 0;
    float c = 0;

    if(rotation == 0)
    {
        // base on bottom
        ax = x_offset;
        ay = y_offset;
        iax = ax + thickness;
        iay = ay + thickness;

        bx = x_offset + width;
        by = y_offset;
        ibx = bx - thickness;
        iby = by + thickness;

        cx = x_offset + (width / 2);
        cy = y_offset + height;
        icx = cx;
        icy = cy - thickness;
    }
    else if(rotation == 1)
    {
        // base on left
        ax = x_offset;
        ay = y_offset;
        iax = ax + thickness;
        iay = ay + thickness;

        bx = x_offset;
        by = y_offset + height;
        ibx = bx + thickness;
        iby = by - thickness;

        cx = x_offset + width;
        cy = y_offset + (height / 2);
        icx = cx - thickness;
        icy = cy;
    }
    else if(rotation == 2)
    {
        // base on top
        ax = x_offset;
        ay = y_offset + height;
        iax = ax + thickness;
        iay = ay - thickness;

        bx = x_offset + width;
        by = y_offset + height;
        ibx = bx - thickness;
        iby = by - thickness;

        cx = x_offset + (width / 2);
        cy = y_offset;
        icx = cx;
        icy = cy + thickness;
    }
    else if(rotation == 3)
    {
        // base on right
        ax = x_offset + width;
        ay = y_offset + height;
        iax = ax - thickness;
        iay = ay - thickness;

        bx = x_offset + width;
        by = y_offset;
        ibx = bx - thickness;
        iby = by + thickness;

        cx = x_offset;
        cy = y_offset + (height / 2);
        icx = cx + thickness;
        icy = cy;
    }
    else
    {
        return;
    }

    for (int x = x_offset; x < x_offset + width; ++x) {
        for (int y = y_offset; y < y_offset + height; ++y) {
            a = (x - bx) * (ay - by) - (ax - bx) * (y - by);
            b = (x - cx) * (by - cy) - (bx - cx) * (y - cy);
            c = (x - ax) * (cy - ay) - (cx - ax) * (y - ay);

            if((a > 0.0 && b > 0.0 && c > 0.0) ||
               (a < 0.0 && b < 0.0 && c < 0.0))
            {
                a = (x - ibx) * (iay - iby) - (iax - ibx) * (y - iby);
                b = (x - icx) * (iby - icy) - (ibx - icx) * (y - icy);
                c = (x - iax) * (icy - iay) - (icx - iax) * (y - iay);

                if((a > 0.0 && b > 0.0 && c > 0.0) ||
                   (a < 0.0 && b < 0.0 && c < 0.0))
                {
                    st7789_set_pixel_xy(bgcolour, x, y, 1);
                }
                else
                {
                    st7789_set_pixel_xy(fgcolour, x, y, 1);
                }
            }
        }
    }
}

inline void st7789_draw_ellipse(uint16_t width, uint16_t height, uint16_t x_offset, uint16_t y_offset, uint16_t thickness)
{
    const float x_radius = width / 2;
    const float y_radius = height / 2;
    const float x_centre = x_offset + x_radius;
    const float y_centre = y_offset + y_radius;

    if(thickness >= x_radius)
    {
        thickness = x_radius;
    }

    if(thickness >= y_radius)
    {
        thickness = y_radius;
    }

    for (int x = x_offset; x < x_offset + width; ++x) {
        for (int y = y_offset; y < y_offset + height; ++y) {
            if(((x - x_centre) * (x - x_centre) / (x_radius * x_radius)) + 
               ((y - y_centre) * (y - y_centre) / (y_radius * y_radius)) <= 1)
            {
                if(((x - x_centre) * (x - x_centre) / ((x_radius - thickness) * (x_radius - thickness))) + 
                   ((y - y_centre) * (y - y_centre) / ((y_radius - thickness) * (y_radius - thickness))) <= 1)
                {
                    st7789_set_pixel_xy(bgcolour, x, y, 1);
                }
                else
                {
                    st7789_set_pixel_xy(fgcolour, x, y, 1);
                }
            }
        }
    }
}


inline void st7789_draw_image(const uint8_t* image_ptr, uint16_t width, uint16_t height, uint16_t x_offset, uint16_t y_offset)
{
    st7789_set_window(x_offset, width - 1, y_offset, height - 1);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t red   = image_ptr[((y + (height - x) * height) * 3) + 0];
            uint8_t green = image_ptr[((y + (height - x) * height) * 3) + 1];
            uint8_t blue  = image_ptr[((y + (height - x) * height) * 3) + 2];
            st7789_set_pixel(st7789_rgb_to_colour(red, green, blue));
        }
    }
}


