#pragma once

#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#define PIO_COUNT 2
#define SM_COUNT 4

#define VSYNC_TIMING 226569695
#define HSYNC_TIMING 119476436

#define INDEX(i, j) (((i) * 640 + (j)) / 5)
#define OFFSET(i, j) ((((i) * 640 + (j)) % 5) * 6)

// Macro to access an element
#define board_get(i, j) (((pixel_data[INDEX(i, j)] >> (26 - OFFSET(i, j))) & 0x3F))

// Macro to modify an element
#define board_set(i, j, k) \
    (pixel_data[INDEX(i, j)] = (pixel_data[INDEX(i, j)] & ~(0x3F << (26 - OFFSET(i, j)))) | ((k & 0x3F) << (26 - OFFSET(i, j))))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))