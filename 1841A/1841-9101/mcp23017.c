// ---------------------------------------------------------------------------------
// MCP23017 I2C expander routines
// ---------------------------------------------------------------------------------
// Routines reading and writing data from an MCP23017 I2C expander
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "mcp23017.h"


// ---------------------------------------------------------------------------------
// Writes a value to an MPC23017 IO expander register over I2C
// ---------------------------------------------------------------------------------
int mcp23017_write_register(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_value)
{
    uint8_t txdata[2];
    int     rval = PICO_ERROR_NONE;

    txdata[0] = reg_addr;
    txdata[1] = reg_value;

    rval = i2c_write_blocking(i2c, i2c_addr, txdata, 2, false);

    return rval;
}

// ---------------------------------------------------------------------------------
// Reads a value from a MPC23017 IO expander register over I2C
// ---------------------------------------------------------------------------------
int mcp23017_read_register(i2c_inst_t* i2c, uint8_t i2c_addr, uint8_t reg_addr, uint8_t* reg_value)
{
    int rval = PICO_ERROR_NONE;

    rval = i2c_write_blocking(i2c, i2c_addr, &reg_addr, 1, true);

    if(rval >= 0)
    {
        rval = i2c_read_blocking(i2c, i2c_addr, reg_value, 1, false);
    }

    return rval;
}
