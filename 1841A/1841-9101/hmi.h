// ---------------------------------------------------------------------------------
// Front panel display routines - Header
// ---------------------------------------------------------------------------------
// Routines for displaying user interfaces on various power simulator front panels
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

// Utility routines
void init_gen_screens(void);
void update_gen_screens(void);
bool draw_gen_leds(struct repeating_timer *t);

void init_buttons(void);
