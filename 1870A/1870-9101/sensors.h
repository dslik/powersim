// ---------------------------------------------------------------------------------
// Sensor registration routines - Header
// ---------------------------------------------------------------------------------
// Routines for managing device sensors
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include <pico/stdlib.h>

// Utility routines
void sensors_initialize_device(void);
void sensors_initialize_hmi(void);

bool draw_anc_leds(struct repeating_timer *t);
