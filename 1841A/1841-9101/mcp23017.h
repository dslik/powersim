// ---------------------------------------------------------------------------------
// MCP23017 I2C expander routines - headers
// ---------------------------------------------------------------------------------
// Routines reading and writing data from an MCP23017 I2C expander
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "hardware/i2c.h"

// Prototypes
int mcp23017_write_register(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_value);
int mcp23017_read_register(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t reg_addr, uint8_t* reg_value);

// Defines
#define	REG_IODIRA		0x00
#define	REG_IODIRB		0x01
#define	REG_IPOLA		0x02
#define	REG_IPOLB		0x03
#define	REG_GPINTENA	0x04
#define	REG_GPINTENB	0x05
#define	REG_DEFVALA		0x06
#define	REG_DEFVALB		0x07
#define	REG_INTCONA		0x08
#define	REG_INTCONB		0x09
#define	REG_IOCON		0x0A	// Same for 0x0B
#define	REG_GPPUA		0x0C
#define	REG_GPPUB		0x0D
#define	REG_INTFA		0x0E
#define	REG_INTFB		0x0F
#define	REG_INTCAPA		0x10
#define	REG_INTCAPB		0x11
#define	REG_GPIOA		0x12
#define	REG_GPIOB		0x13
#define	REG_OLATA		0x14
#define	REG_OLATB		0x15