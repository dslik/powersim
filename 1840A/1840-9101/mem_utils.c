// ---------------------------------------------------------------------------------
// Memory utilities
// ---------------------------------------------------------------------------------
// Get free memory
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright (c) 2021-2023 Christophe "CHiPs44" Petit
// SPDX-FileAttributionText: https://github.com/CHiPs44/hagl_pico_vgaboard/blob/main/example/specs.c
// SPDX-License-Identifier: MIT-0
// ---------------------------------------------------------------------------------
#include <stdio.h>
#include <malloc.h>

/**
 * @brief Get free RAM using static memory defines
 *        cf. https://forums.raspberrypi.com/viewtopic.php?t=347638#p2082565
 * 
 * @return size_t 
 */
size_t get_free_ram_2(void)
{
   extern char __StackLimit, __bss_end__;
   size_t total_heap = &__StackLimit  - &__bss_end__;
   struct mallinfo info = mallinfo();
   return total_heap - info.uordblks;
}