// ---------------------------------------------------------------------------------
// ASM HMI Helper Resources - Header
// ---------------------------------------------------------------------------------
// Constants and routines to assist with creating HMIs that follow the HMI
// guidelines of the ASM consortium.
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "pico/stdlib.h"

// Colours
#define	asm_text		0,   0,   0
#define	asm_bg_grey		204, 204, 204
#define	asm_line_grey	145, 145, 145
#define	asm_fill_grey	102, 102, 102

// Alarm severities
#define	asm_alarm_one	1
#define	asm_alarm_two 	2
#define	asm_alarm_three 2

// Alarm colours
#define	asm_alarm_one_colour	58,  142, 237
#define	asm_alarm_two_colour	223, 126, 0

// Flows
#define	asm_flow_none	0
#define	asm_flow_up		1
#define	asm_flow_down	2


// Prototypes
void asm_draw_flow_arrow(uint8_t phase, uint8_t direction);
void asm_draw_flow_value(uint8_t phase, float value, char* units);
void asm_draw_value_alarm(uint8_t phase, uint8_t severity);
void asm_draw_value_indicator(uint8_t phase, float ll, float l, float value, float h, float hh, float sp);
