#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "VGA.h"
// Our assembled program:
#include "VGA_ASM.pio.h"
#include <stdio.h>
#include "pixel_data.h"

int PRIMARY_DMA_CHAN;

// triggered after DMA transfers one frame.
void __not_in_flash_func(pioIRQ_handler)(){
    dma_hw->ints0 = 1u << PRIMARY_DMA_CHAN;

    while(!(pio0_hw->irq & 4)){}        // & 4 means wait for IRQ 2. 
    dma_hw->ch[PRIMARY_DMA_CHAN].al3_read_addr_trig = pixel_data;
    pio_interrupt_clear(pio0, 2);
}

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

	uint VGA_LINE = 2;

    set_sys_clock_pll(1512000000, 6, 2);

    int sm_vsync = pio_claim_unused_sm(pio, true);
    int sm_hsync = pio_claim_unused_sm(pio, true);
    int sm = pio_claim_unused_sm(pio, true);
    int sm_flag_off = pio_claim_unused_sm(pio, true);

    VGACOM_program_init(pio, sm, VGA_LINE, TX_START, TX_WRAP_TARGET, TX_WRAP, 1.0);

    VGASYNC_program_init(pio, sm_hsync, VGA_LINE + 6, SYNC_START, SYNC_WRAP_TARGET, SYNC_WRAP, 5.0, VGA_LINE);
    pio_sm_put_blocking(pio, sm_hsync, HSYNC_TIMING);
    pio_sm_put_blocking(pio, sm_hsync, 41026);  // nop

    VGASYNC_program_init(pio, sm_vsync, VGA_LINE + 7, SYNC_START, SYNC_WRAP_TARGET, SYNC_WRAP-1, 1.0, VGA_LINE);
    pio_sm_put_blocking(pio, sm_vsync, VSYNC_TIMING);
    pio_sm_put_blocking(pio, sm_vsync, 8390);  // WAIT 1 IRQ 6
	
    PRIMARY_DMA_CHAN = dma_claim_unused_channel(true);

    uint pio_fifo_dreq = pio_get_dreq(pio, sm, true);

    dma_channel_config chan_config = get_dma_common_config(PRIMARY_DMA_CHAN, true, false, pio_fifo_dreq, -1);
    dma_channel_configure(PRIMARY_DMA_CHAN, &chan_config, &pio->txf[sm], pixel_data, 61440, true);
    dma_channel_set_irq0_enabled(PRIMARY_DMA_CHAN, true);
    irq_set_exclusive_handler(DMA_IRQ_0, pioIRQ_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    VGAFLAG_program_init(pio, sm_flag_off, 0, 0, 0, 1.0);

    while (true)
        tight_loop_contents();
}