// ---------------------------------------------------------------------------------
// Font Utility Routines
// ---------------------------------------------------------------------------------
// Library routines to assist displaying text on a display
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/rack-display/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "fonts.h"
#include "st7789_lcd.h"

// Local Prototypes
const char* character_to_array(char character, uint8_t typeface);
const uint8_t character_to_width(char character, uint8_t typeface);
const uint8_t character_to_height(char character, uint8_t typeface);


// =================================================================================

int16_t st7789_draw_character(char character, uint8_t typeface, uint16_t x_offset, uint16_t y_offset)
{
	const uint8_t*	character_data = character_to_array(character, typeface);
	const uint8_t 	character_width = character_to_width(character, typeface);
	const uint8_t 	character_height = character_to_height(character, typeface);
	uint16_t counter = 0;
	uint8_t row_counter = 0;
	uint8_t col_counter = 0;

	float 	pixel_value = 0;
	float	bg_red = st7789_colour_to_red(st7789_get_bgcolor());
	float	bg_green = st7789_colour_to_green(st7789_get_bgcolor());
	float	bg_blue = st7789_colour_to_blue(st7789_get_bgcolor());
	float	fg_red = st7789_colour_to_red(st7789_get_fgcolor());
	float	fg_green = st7789_colour_to_green(st7789_get_fgcolor());
	float	fg_blue = st7789_colour_to_blue(st7789_get_fgcolor());

	if(character_data != NULL)
	{
   	    for (int x = 0; x < character_width; ++x) {
		    for (int y = 0; y < character_height; ++y) {
				pixel_value = character_data[x * character_height + y];
        	    st7789_set_pixel_xy(st7789_rgb_to_colour((pixel_value) * (bg_red / 255.0) + (255.0 - pixel_value) * (fg_red / 255.0),
        	    	                                     (pixel_value) * (bg_green / 255.0) + (255.0 - pixel_value) * (fg_green / 255.0), 
        	    	                                     (pixel_value) * (bg_blue / 255.0) + (255.0 - pixel_value) * (fg_blue / 255.0)),
        	    	                                     x + x_offset, y + y_offset, 1);
        	}
        }
	}

	return(x_offset + character_width);
}


void st7789_draw_string(char* string, uint8_t typeface, uint16_t x_offset, uint16_t y_offset)
{
	uint16_t counter = 0;
	uint16_t offset = x_offset;

	while(string[counter] != 0)
	{
		offset = st7789_draw_character(string[counter], typeface, offset, y_offset);
		counter = counter + 1;
	}
}

void st7789_draw_string_centred(char* string, uint8_t typeface, uint16_t x_start, uint16_t x_end, uint16_t y_offset)
{
	uint16_t counter = 0;
	uint16_t offset = st7789_centre_string(string, typeface, x_start, x_end);
    

	while(string[counter] != 0)
	{
		offset = st7789_draw_character(string[counter], typeface, offset, y_offset);
		counter = counter + 1;
	}
}

uint16_t st7789_centre_string(char* string, uint8_t typeface, uint16_t x_start, uint16_t x_end)
{
	uint16_t counter = 0;
	uint16_t offset = 0;

	// Measure how long the string is
	while(string[counter] != 0)
	{
		offset = offset + character_to_width(string[counter], typeface);
		counter = counter + 1;
	}

	return(x_start + ((x_end - x_start) / 2) - (offset / 2));
}

const char* character_to_array(char character, uint8_t typeface)
{
	if(typeface == B612_BMA_24)
	{
		if(character == ' ') return(glyph_b612_bma_24_space);

		if(character == 'a') return(glyph_b612_bma_24_a);
		if(character == 'b') return(glyph_b612_bma_24_b);
		if(character == 'c') return(glyph_b612_bma_24_c);
		if(character == 'd') return(glyph_b612_bma_24_d);
		if(character == 'e') return(glyph_b612_bma_24_e);
		if(character == 'f') return(glyph_b612_bma_24_f);
		if(character == 'g') return(glyph_b612_bma_24_g);
		if(character == 'h') return(glyph_b612_bma_24_h);
		if(character == 'i') return(glyph_b612_bma_24_i);
		if(character == 'j') return(glyph_b612_bma_24_j);
		if(character == 'k') return(glyph_b612_bma_24_k);
		if(character == 'l') return(glyph_b612_bma_24_l);
		if(character == 'm') return(glyph_b612_bma_24_m);
		if(character == 'n') return(glyph_b612_bma_24_n);
		if(character == 'o') return(glyph_b612_bma_24_o);
		if(character == 'p') return(glyph_b612_bma_24_p);
		if(character == 'q') return(glyph_b612_bma_24_q);
		if(character == 'r') return(glyph_b612_bma_24_r);
		if(character == 's') return(glyph_b612_bma_24_s);
		if(character == 't') return(glyph_b612_bma_24_t);
		if(character == 'u') return(glyph_b612_bma_24_u);
		if(character == 'v') return(glyph_b612_bma_24_v);
		if(character == 'w') return(glyph_b612_bma_24_w);
		if(character == 'x') return(glyph_b612_bma_24_x);
		if(character == 'y') return(glyph_b612_bma_24_y);
		if(character == 'z') return(glyph_b612_bma_24_z);

		if(character == 'A') return(glyph_b612_bma_24_A);
		if(character == 'B') return(glyph_b612_bma_24_B);
		if(character == 'C') return(glyph_b612_bma_24_C);
		if(character == 'D') return(glyph_b612_bma_24_D);
		if(character == 'E') return(glyph_b612_bma_24_E);
		if(character == 'F') return(glyph_b612_bma_24_F);
		if(character == 'G') return(glyph_b612_bma_24_G);
		if(character == 'H') return(glyph_b612_bma_24_H);
		if(character == 'I') return(glyph_b612_bma_24_I);
		if(character == 'J') return(glyph_b612_bma_24_J);
		if(character == 'K') return(glyph_b612_bma_24_K);
		if(character == 'L') return(glyph_b612_bma_24_L);
		if(character == 'M') return(glyph_b612_bma_24_M);
		if(character == 'N') return(glyph_b612_bma_24_N);
		if(character == 'O') return(glyph_b612_bma_24_O);
		if(character == 'P') return(glyph_b612_bma_24_P);
		if(character == 'Q') return(glyph_b612_bma_24_Q);
		if(character == 'R') return(glyph_b612_bma_24_R);
		if(character == 'S') return(glyph_b612_bma_24_S);
		if(character == 'T') return(glyph_b612_bma_24_T);
		if(character == 'U') return(glyph_b612_bma_24_U);
		if(character == 'V') return(glyph_b612_bma_24_V);
		if(character == 'W') return(glyph_b612_bma_24_W);
		if(character == 'X') return(glyph_b612_bma_24_X);
		if(character == 'Y') return(glyph_b612_bma_24_Y);
		if(character == 'Z') return(glyph_b612_bma_24_Z);

		if(character == '0') return(glyph_b612_bma_24_0);
		if(character == '1') return(glyph_b612_bma_24_1);
		if(character == '2') return(glyph_b612_bma_24_2);
		if(character == '3') return(glyph_b612_bma_24_3);
		if(character == '4') return(glyph_b612_bma_24_4);
		if(character == '5') return(glyph_b612_bma_24_5);
		if(character == '6') return(glyph_b612_bma_24_6);
		if(character == '7') return(glyph_b612_bma_24_7);
		if(character == '8') return(glyph_b612_bma_24_8);
		if(character == '9') return(glyph_b612_bma_24_9);

		if(character == '%') return(glyph_b612_bma_24_percent);
		if(character == '&') return(glyph_b612_bma_24_ampersand);
		if(character == '(') return(glyph_b612_bma_24_open_paren);
		if(character == ')') return(glyph_b612_bma_24_close_paren);
		if(character == '-') return(glyph_b612_bma_24_minus);
		if(character == '=') return(glyph_b612_bma_24_equal);
		if(character == '+') return(glyph_b612_bma_24_plus);
		if(character == '?') return(glyph_b612_bma_24_question);
		if(character == 230) return(glyph_b612_bma_24_micro);
		if(character == '.') return(glyph_b612_bma_24_period);
		if(character == '/') return(glyph_b612_bma_24_slash);
		if(character == '<') return(glyph_b612_bma_24_less);
		if(character == '>') return(glyph_b612_bma_24_greater);
	}

	if(typeface == B612_BMA_32)
	{
		if(character == ' ') return(glyph_b612_bma_32_space);

		if(character == 'a') return(glyph_b612_bma_32_a);
		if(character == 'b') return(glyph_b612_bma_32_b);
		if(character == 'c') return(glyph_b612_bma_32_c);
		if(character == 'd') return(glyph_b612_bma_32_d);
		if(character == 'e') return(glyph_b612_bma_32_e);
		if(character == 'f') return(glyph_b612_bma_32_f);
		if(character == 'g') return(glyph_b612_bma_32_g);
		if(character == 'h') return(glyph_b612_bma_32_h);
		if(character == 'i') return(glyph_b612_bma_32_i);
		if(character == 'j') return(glyph_b612_bma_32_j);
		if(character == 'k') return(glyph_b612_bma_32_k);
		if(character == 'l') return(glyph_b612_bma_32_l);
		if(character == 'm') return(glyph_b612_bma_32_m);
		if(character == 'n') return(glyph_b612_bma_32_n);
		if(character == 'o') return(glyph_b612_bma_32_o);
		if(character == 'p') return(glyph_b612_bma_32_p);
		if(character == 'q') return(glyph_b612_bma_32_q);
		if(character == 'r') return(glyph_b612_bma_32_r);
		if(character == 's') return(glyph_b612_bma_32_s);
		if(character == 't') return(glyph_b612_bma_32_t);
		if(character == 'u') return(glyph_b612_bma_32_u);
		if(character == 'v') return(glyph_b612_bma_32_v);
		if(character == 'w') return(glyph_b612_bma_32_w);
		if(character == 'x') return(glyph_b612_bma_32_x);
		if(character == 'y') return(glyph_b612_bma_32_y);
		if(character == 'z') return(glyph_b612_bma_32_z);

		if(character == 'A') return(glyph_b612_bma_32_A);
		if(character == 'B') return(glyph_b612_bma_32_B);
		if(character == 'C') return(glyph_b612_bma_32_C);
		if(character == 'D') return(glyph_b612_bma_32_D);
		if(character == 'E') return(glyph_b612_bma_32_E);
		if(character == 'F') return(glyph_b612_bma_32_F);
		if(character == 'G') return(glyph_b612_bma_32_G);
		if(character == 'H') return(glyph_b612_bma_32_H);
		if(character == 'I') return(glyph_b612_bma_32_I);
		if(character == 'J') return(glyph_b612_bma_32_J);
		if(character == 'K') return(glyph_b612_bma_32_K);
		if(character == 'L') return(glyph_b612_bma_32_L);
		if(character == 'M') return(glyph_b612_bma_32_M);
		if(character == 'N') return(glyph_b612_bma_32_N);
		if(character == 'O') return(glyph_b612_bma_32_O);
		if(character == 'P') return(glyph_b612_bma_32_P);
		if(character == 'Q') return(glyph_b612_bma_32_Q);
		if(character == 'R') return(glyph_b612_bma_32_R);
		if(character == 'S') return(glyph_b612_bma_32_S);
		if(character == 'T') return(glyph_b612_bma_32_T);
		if(character == 'U') return(glyph_b612_bma_32_U);
		if(character == 'V') return(glyph_b612_bma_32_V);
		if(character == 'W') return(glyph_b612_bma_32_W);
		if(character == 'X') return(glyph_b612_bma_32_X);
		if(character == 'Y') return(glyph_b612_bma_32_Y);
		if(character == 'Z') return(glyph_b612_bma_32_Z);

		if(character == '0') return(glyph_b612_bma_32_0);
		if(character == '1') return(glyph_b612_bma_32_1);
		if(character == '2') return(glyph_b612_bma_32_2);
		if(character == '3') return(glyph_b612_bma_32_3);
		if(character == '4') return(glyph_b612_bma_32_4);
		if(character == '5') return(glyph_b612_bma_32_5);
		if(character == '6') return(glyph_b612_bma_32_6);
		if(character == '7') return(glyph_b612_bma_32_7);
		if(character == '8') return(glyph_b612_bma_32_8);
		if(character == '9') return(glyph_b612_bma_32_9);

		if(character == '%') return(glyph_b612_bma_32_percent);
		if(character == '&') return(glyph_b612_bma_32_ampersand);
		if(character == '(') return(glyph_b612_bma_32_open_paren);
		if(character == ')') return(glyph_b612_bma_32_close_paren);
		if(character == '-') return(glyph_b612_bma_32_minus);
		if(character == '=') return(glyph_b612_bma_32_equal);
		if(character == '+') return(glyph_b612_bma_32_plus);
		if(character == '?') return(glyph_b612_bma_32_question);
		if(character == 230) return(glyph_b612_bma_32_micro);
		if(character == '.') return(glyph_b612_bma_32_period);
		if(character == '/') return(glyph_b612_bma_32_slash);
		if(character == '<') return(glyph_b612_bma_32_less);
		if(character == '>') return(glyph_b612_bma_32_greater);
	}

	return(NULL);
}

const uint8_t character_to_width(char character, uint8_t typeface)
{
	if(typeface == B612_BMA_24)
	{
		return(15);
	}

	if(typeface == B612_BMA_32)
	{
		return(21);
	}

	return(0);		
}

const uint8_t character_to_height(char character, uint8_t typeface)
{
	if(typeface == B612_BMA_24)
	{
		return(27);
	}

	if(typeface == B612_BMA_32)
	{
		return(36);
	}

	return(0);		
}