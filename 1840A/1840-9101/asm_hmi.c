// ---------------------------------------------------------------------------------
// ASM HMI Helper Resources
// ---------------------------------------------------------------------------------
// Utility routines to assist with creating HMIs that follow the HMI guidelines of
// the ASM consortium.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include <math.h>

#include "asm_hmi.h"
#include "st7789_lcd.h"
#include "fonts.h"

void asm_draw_flow_arrow(uint8_t phase, uint8_t direction)
{
	uint16_t	bg_color = st7789_get_bgcolor();
	uint16_t	fg_color = st7789_get_fgcolor();

	if(phase == 0 || phase > 3)
	{
		phase = 1;
	}

    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));

	if(direction == asm_flow_none)
	{
	    st7789_draw_rect(5, 30, (SCREEN_WIDTH / 3) * (phase - 1) + (SCREEN_WIDTH / 6) - 2, 10, 5);
	}

	if(direction == asm_flow_up)
	{
	    st7789_draw_rect(5, 20, (SCREEN_WIDTH / 3) * (phase - 1) + (SCREEN_WIDTH / 6) - 2, 10, 5);
    	st7789_set_bgcolor(st7789_rgb_to_colour(asm_line_grey));
    	st7789_draw_triangle(18, 24, (SCREEN_WIDTH / 3) * (phase - 1) + (SCREEN_WIDTH / 6) - 9, 16, 0, 0);
	}

	if(direction == asm_flow_down)
	{
	    st7789_draw_rect(5, 20, (SCREEN_WIDTH / 3) * (phase - 1) + (SCREEN_WIDTH / 6) - 2, 20, 5);
    	st7789_set_bgcolor(st7789_rgb_to_colour(asm_line_grey));
    	st7789_draw_triangle(18, 24, (SCREEN_WIDTH / 3) * (phase - 1) + (SCREEN_WIDTH / 6) - 9, 10, 2, 0);
	}

	st7789_set_bgcolor(bg_color);
	st7789_set_fgcolor(fg_color);
}

void asm_draw_value_indicator(uint8_t phase, float ll, float l, float value, float h, float hh, float sp)
{
	uint16_t	bg_color = st7789_get_bgcolor();
	uint16_t	fg_color = st7789_get_fgcolor();
	uint16_t	x_offset = (SCREEN_WIDTH / 3) * (phase - 1);
	uint16_t	y_offset = 125;
	float		span = hh - ll;
	float		ratio = 60 / span;
	float		inner_start = (l - ll) * ratio;
	float		inner_width = (h - l) * ratio;

	if(phase == 0 || phase > 3)
	{
		phase = 1;
	}

	// Peg at hh/ll
    if(value > hh)
    {
    	value = hh;
    }

    if(value < ll)
    {
    	value = ll;
    }

    if(sp > hh)
    {
    	sp = hh;
    }

    if(sp < ll)
    {
    	sp = ll;
    }

	float		x_sp = (sp - l) * ratio;
	float		x_value = (value - l) * ratio;

	st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_line_grey));

    // Draw indicator
	st7789_draw_rect(64, 15, x_offset + 8, y_offset, 2);
	st7789_draw_rect(inner_width + 2, 15, x_offset + 8 + inner_start - 1, y_offset, 2);

	// Draw setpoint
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_line_grey));
    st7789_draw_triangle(12, 12, x_offset + 8 + 10 + x_sp, y_offset + 13, 2, 0);

	// Draw value
    st7789_set_bgcolor(st7789_rgb_to_colour(asm_fill_grey));
    st7789_draw_triangle(12, 12, x_offset + 8 + 10 + x_value, y_offset + 13, 2, 0);

	st7789_set_bgcolor(bg_color);
	st7789_set_fgcolor(fg_color);
}

void asm_draw_flow_value(uint8_t phase, float value, char* units)
{
	uint16_t	bg_color = st7789_get_bgcolor();
	uint16_t	fg_color = st7789_get_fgcolor();
    char        snprintf_buffer[10];
    const char*	prefix;
    const char* format;
    float		converted_value;

	if(phase == 0 || phase > 3)
	{
		phase = 1;
	}

	if(value >= 10000000 || value <= -10000000)
	{
		prefix = "M";
		format = "%3f";
		converted_value = round(value / 1000000.0);
	}
	if(value >= 1000000 || value <= -1000000)
	{
		prefix = "M";
		format = "%.1f";
		converted_value = value / 1000000.0;
	}
	else if(value >= 10000 || value <= -10000)
	{
		prefix = "k";
		format = "%.0f";
		converted_value = round(value / 1000.0);
	}
	else if(value >= 1000 || value <= -1000)
	{
		prefix = "k";
		format = "%.1f";
		converted_value = value / 1000.0;
	}
	else if(value >= 10 || value <= -10)
	{
		prefix = "";
		format = "%.0f";
		converted_value = round(value);
	}
	else if(value >= 0.1 || value <= -0.1)
	{
		prefix = "";
		format = "%.1f";
		converted_value = value;
	}
	else if(value > 0.0001 || value < -0.0001)
	{
		prefix = "m";
		format = "%.1f";
		converted_value = value * 1000.0;
	}
	else if(value == 0)
	{
		prefix = "";
		format = "0";
		converted_value = value;
	}
	else
	{
		prefix = "\346";
		format = "%.1f";
		converted_value = value * 1000000.0;
	}

    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));
    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));

    snprintf(snprintf_buffer, 10, format, converted_value);
    st7789_draw_string_centred(snprintf_buffer, B612_BMA_24, (SCREEN_WIDTH / 3) * (phase - 1), (SCREEN_WIDTH / 3) * phase, 90);

    snprintf(snprintf_buffer, 10, "%s%s", prefix, units);
    st7789_draw_string_centred(snprintf_buffer, B612_BMA_24, (SCREEN_WIDTH / 3) * (phase - 1), (SCREEN_WIDTH / 3) * phase, 60);

	st7789_set_bgcolor(bg_color);
	st7789_set_fgcolor(fg_color);
}

void asm_draw_value_alarm(uint8_t phase, uint8_t severity)
{
	uint16_t	bg_color = st7789_get_bgcolor();
	uint16_t	fg_color = st7789_get_fgcolor();

	if(phase == 0 || phase > 3)
	{
		phase = 1;
	}

    st7789_set_bgcolor(st7789_rgb_to_colour(asm_bg_grey));

	if(severity == asm_alarm_one)
	{
	    st7789_set_fgcolor(st7789_rgb_to_colour(asm_alarm_one_colour));
		st7789_set_bg_trans(true);
	    st7789_draw_rect((SCREEN_WIDTH / 3) - 10, 65, (SCREEN_WIDTH / 3) * (phase - 1) + 5, 58, 4);
		st7789_set_bg_trans(false);
	
		if(phase == 1 || phase == 2)
		{
		    st7789_set_bgcolor(st7789_rgb_to_colour(asm_alarm_one_colour));
		    st7789_draw_triangle(40, 40, 60, 10, 2, 0);
		    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
			st7789_set_bg_trans(true);
		    st7789_draw_string_centred("1", B612_BMA_24, 60, 100, 20);
			st7789_set_bg_trans(false);
		}
		else
		{
		    st7789_set_bgcolor(st7789_rgb_to_colour(asm_alarm_one_colour));
		    st7789_draw_triangle(40, 40, 140, 10, 2, 0);
		    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
			st7789_set_bg_trans(true);
		    st7789_draw_string_centred("1", B612_BMA_24, 140, 180, 20);
			st7789_set_bg_trans(false);
		}

	}

	if(severity == asm_alarm_two)
	{
	    st7789_set_fgcolor(st7789_rgb_to_colour(asm_alarm_two_colour));
		st7789_set_bg_trans(true);
	    st7789_draw_rect((SCREEN_WIDTH / 3) - 10, 65, (SCREEN_WIDTH / 3) * (phase - 1) + 5, 58, 4);
		st7789_set_bg_trans(false);
	
		if(phase == 1 || phase == 2)
		{
		    st7789_set_bgcolor(st7789_rgb_to_colour(asm_alarm_two_colour));
			st7789_draw_ellipse(35, 35, 63, 15, 0);
		    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
			st7789_set_bg_trans(true);
		    st7789_draw_string_centred("2", B612_BMA_24, 60, 100, 18);
			st7789_set_bg_trans(false);
		}
		else
		{
		    st7789_set_bgcolor(st7789_rgb_to_colour(asm_alarm_two_colour));
			st7789_draw_ellipse(35, 35, 143, 15, 0);
		    st7789_set_fgcolor(st7789_rgb_to_colour(asm_text));
			st7789_set_bg_trans(true);
		    st7789_draw_string_centred("2", B612_BMA_24, 140, 180, 18);
			st7789_set_bg_trans(false);
		}

	}

	st7789_set_bgcolor(bg_color);
	st7789_set_fgcolor(fg_color);
}