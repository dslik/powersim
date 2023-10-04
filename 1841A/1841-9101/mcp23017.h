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
