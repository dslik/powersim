// ---------------------------------------------------------------------------------
// Pi Pico Pin Assignments - Header
// ---------------------------------------------------------------------------------
// Indicates which pins are assigned to which functions
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// Constants
#define PIN_DIN 			20
#define PIN_CLK 			21
#define PIN_CS_1			25
#define PIN_CS_2 			24
#define PIN_CS_3			23
#define PIN_DC				2
#define PIN_RESET			19
#define PIN_BL				3

#define PIN_FC_CS           10  			// Font IC Chip Select
#define PIN_FC_SO           9   			// Font IC Serial Output
#define PIN_FC_CL           21  			// Font IC Clock

#define I2C_SDA_PIN     	6               // GPIO line to use for SDA
#define I2C_SCL_PIN     	7               // GPIO line to use for SCL

#define	I2C_IRQ_PIN			18				// Interrupt line

#define I2C_ADDR_1      	0b00100000      // I2C address of the target
#define I2C_ADDR_2      	0b00100001      // I2C address of the target
#define I2C_ADDR_3      	0b00100010      // I2C address of the target

#define DEBUG_WS2812 		11  			// Base GPIO for the debug LED
