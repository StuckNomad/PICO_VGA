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
#include "pixel_data.h"

int PRIMARY_DMA_CHAN_1;
int PRIMARY_DMA_CHAN_2;
int TRIGGER_DMA_CHAN_1;
int TRIGGER_DMA_CHAN_2;

dma_channel_config get_dma_common_config(int dma_chan, bool read_incr, bool write_incr, uint dreq, uint trigger_chan){
    dma_channel_config config = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, read_incr);
    channel_config_set_write_increment(&config, write_incr);
    if(dreq != -1)  channel_config_set_dreq(&config, dreq);
    if(trigger_chan != -1)  channel_config_set_chain_to(&config, trigger_chan);
    return config;
}

void __not_in_flash_func(pioIRQ_handler)(){
    // pio_interrupt_clear(pio0, 2);
    dma_hw->ints0 = 1u << PRIMARY_DMA_CHAN_1;
    // dma_channel_abort(PRIMARY_DMA_CHAN_1);
    // dma_channel_cleanup(PRIMARY_DMA_CHAN_1);
    // dma_channel_abort(TRIGGER_DMA_CHAN_2);
    // dma_channel_cleanup(TRIGGER_DMA_CHAN_2);
    // dma_channel_start(TRIGGER_DMA_CHAN_2);

    while(!(pio0_hw->irq & 4)){}        // & 4 means wait for IRQ 2. 
    dma_hw->ch[PRIMARY_DMA_CHAN_1].al3_read_addr_trig = pixel_data;
    pio_interrupt_clear(pio0, 2);
}

int main() {

    stdio_init_all();
    PIO pio = pio0;
    pio_add_program_at_offset(pio, &VGACOM_program, 0);

	uint VGA_LINE = 2;

    set_sys_clock_pll(1512000000, 6, 2);

    int sm_vsync = pio_claim_unused_sm(pio, true);
    int sm_hsync = pio_claim_unused_sm(pio, true);
    int sm = pio_claim_unused_sm(pio, true);
    int sm_flag_off = pio_claim_unused_sm(pio, true);

    // pio_set_irq0_source_mask_enabled(pio0, 512, true);
    // irq_set_exclusive_handler(pio_get_irq_num(pio, 0), pioIRQ_handler);
    // irq_set_enabled(pio_get_irq_num(pio, 0), true);

    VGACOM_program_init(pio, sm, VGA_LINE, TX_START, TX_WRAP_TARGET, TX_WRAP, 1.0);

    VGASYNC_program_init(pio, sm_hsync, VGA_LINE + 6, SYNC_START, SYNC_WRAP_TARGET, SYNC_WRAP, 5.0, VGA_LINE);
    pio_sm_put_blocking(pio, sm_hsync, 119476436);
    pio_sm_put_blocking(pio, sm_hsync, 41026);

    VGASYNC_program_init(pio, sm_vsync, VGA_LINE + 7, SYNC_START, SYNC_WRAP_TARGET, SYNC_WRAP-1, 1.0, VGA_LINE);
    pio_sm_put_blocking(pio, sm_vsync, 268510687);
    pio_sm_put_blocking(pio, sm_vsync, 8390);
	
    PRIMARY_DMA_CHAN_1 = dma_claim_unused_channel(true);
    PRIMARY_DMA_CHAN_2 = dma_claim_unused_channel(true);
    TRIGGER_DMA_CHAN_1 = dma_claim_unused_channel(true);
    TRIGGER_DMA_CHAN_2 = dma_claim_unused_channel(true);

    uint pio_fifo_dreq = pio_get_dreq(pio, sm, true);

    uint32_t bit_array_1[4] = {2576980377,2576980377,2576980377};
    // uint32_t bit_array_2[4] = {3284386755,3284386755,3284386755};

    uint32_t* bit_array_1_addr = pixel_data;
    // uint32_t* bit_array_2_addr = bit_array_2;

    dma_channel_config chan_config_1 = get_dma_common_config(PRIMARY_DMA_CHAN_1, true, false, pio_fifo_dreq, -1);
    dma_channel_configure(PRIMARY_DMA_CHAN_1, &chan_config_1, &pio->txf[sm], pixel_data, 61440, true);
    dma_channel_set_irq0_enabled(PRIMARY_DMA_CHAN_1, true);
    irq_set_exclusive_handler(DMA_IRQ_0, pioIRQ_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // dma_channel_config chan_config_2 = get_dma_common_config(PRIMARY_DMA_CHAN_2, true, false, pio_fifo_dreq, TRIGGER_DMA_CHAN_1);
    // dma_channel_configure(PRIMARY_DMA_CHAN_2, &chan_config_2, &pio->txf[sm], NULL, 3, false);

    // dma_channel_config chan_config_3 = get_dma_common_config(TRIGGER_DMA_CHAN_1, false, false, -1 , -1);
    // dma_channel_configure(
    //     TRIGGER_DMA_CHAN_1,
    //     &chan_config_3,
    //     &dma_hw->ch[PRIMARY_DMA_CHAN_1].al3_read_addr_trig,// Write address
    //     &bit_array_1_addr,             // provide a read address
    //     1,          // Write all values in the array
    //     false                    // start
    // );

    // dma_channel_config chan_config_4 = get_dma_common_config(TRIGGER_DMA_CHAN_2, false, false, -1, -1);;
    // dma_channel_configure(
    //     TRIGGER_DMA_CHAN_2,
    //     &chan_config_4,
    //     &dma_hw->ch[PRIMARY_DMA_CHAN_1].al3_read_addr_trig,// Write address
    //     &bit_array_1_addr,             // provide a read address
    //     1,          // Write all values in the array
    //     false                    // start
    // );

    // pioIRQ_handler();
    // dma_channel_start(TRIGGER_DMA_CHAN_2);

    VGASYNC_program_init(pio, sm_flag_off, VGA_LINE, 0, 0, 0, 1.0, VGA_LINE);

    while (true)
        tight_loop_contents();
}