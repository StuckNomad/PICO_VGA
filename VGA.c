/**
 *C program for Ep 7, Ex 1, blink LED with unconditional Jump
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
// Our assembled program:
#include "VGA_ASM.pio.h"
#include <stdio.h>

dma_channel_config get_dma_common_config(int dma_chan, bool read_incr, bool write_incr, uint dreq, uint trigger_chan){
    dma_channel_config config = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, read_incr);
    channel_config_set_write_increment(&config, write_incr);
    if(dreq != -1)  channel_config_set_dreq(&config, dreq);
    if(trigger_chan != -1)  channel_config_set_chain_to(&config, trigger_chan);
    return config;
}

int main() {

    stdio_init_all();
    PIO pio = pio0;
    pio_add_program_at_offset(pio, &VGACOM_program, 0);

	uint USB_LINE = 2;

    set_sys_clock_pll(1512000000, 6 ,2);

    int sm_vsync = pio_claim_unused_sm(pio, true);
    int sm_hsync = pio_claim_unused_sm(pio, true);
    int sm = pio_claim_unused_sm(pio, true);

    VGACOM_program_init(pio, sm, USB_LINE, 0, 0, 0, 5.0);

    VGASYNC_program_init(pio, sm_hsync, USB_LINE + 2, 1, 5, 20, 5.0);
    pio_sm_put_blocking(pio, sm_hsync, 119476436);
    pio_sm_put_blocking(pio, sm_hsync, 41026);

    VGASYNC_program_init(pio, sm_vsync, USB_LINE + 3, 1, 5, 19, 1.0);
    pio_sm_put_blocking(pio, sm_vsync, 268510687);
    pio_sm_put_blocking(pio, sm_vsync, 8386);
	
    int primary_dma_chan_1 = dma_claim_unused_channel(true);
    int primary_dma_chan_2 = dma_claim_unused_channel(true);
    int trigger_dma_chan_1 = dma_claim_unused_channel(true);
    int trigger_dma_chan_2 = dma_claim_unused_channel(true);

    uint pio_fifo_dreq = pio_get_dreq(pio, sm, true);

    uint32_t bit_array_1[4] = {2576980377,2576980377,2576980377};
    uint32_t bit_array_2[4] = {3284386755,3284386755,3284386755};

    uint32_t* bit_array_1_addr = bit_array_1;
    uint32_t* bit_array_2_addr = bit_array_2;

    dma_channel_config chan_config_1 = get_dma_common_config(primary_dma_chan_1, true, false, pio_fifo_dreq, trigger_dma_chan_2);
    dma_channel_configure(primary_dma_chan_1, &chan_config_1, &pio->txf[sm], NULL, 3, false);

    dma_channel_config chan_config_2 = get_dma_common_config(primary_dma_chan_2, true, false, pio_fifo_dreq, trigger_dma_chan_1);
    dma_channel_configure(primary_dma_chan_2, &chan_config_2, &pio->txf[sm], NULL, 3, false);

    dma_channel_config chan_config_3 = get_dma_common_config(trigger_dma_chan_1, false, false, -1 , -1);
    dma_channel_configure(
        trigger_dma_chan_1,
        &chan_config_3,
        &dma_hw->ch[primary_dma_chan_1].al3_read_addr_trig,// Write address
        &bit_array_1_addr,             // provide a read address
        1,          // Write all values in the array
        false                    // start
    );

    dma_channel_config chan_config_4 = get_dma_common_config(trigger_dma_chan_2, false, false, -1, -1);;
    dma_channel_configure(
        trigger_dma_chan_2,
        &chan_config_4,
        &dma_hw->ch[primary_dma_chan_2].al3_read_addr_trig,// Write address
        &bit_array_2_addr,             // provide a read address
        1,          // Write all values in the array
        true                    // start
    );
}