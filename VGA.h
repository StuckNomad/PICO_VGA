#pragma once

#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#define PIO_COUNT 2
#define SM_COUNT 4

typedef struct {
    int base_pin;                    // pin corresponding to D- line of USB. base_pin+1 would be D+
    PIO pio;                        // the pio assigned to this port
    int sm;                         // the state machine assigned to this port
    int dma_chan;  
    bool is_fullSpeed;                 // the dma channel assigned to this port for TX/RX operations.
    dma_channel_config dma_TX_config;   // pre-defined TX configuration for the assigned dma channel
    dma_channel_config dma_RX_config;   // pre-defined TX configuration for the assigned dma channel
    pio_sm_config sm_config;             // config for pio sm 
    uint32_t RxBuffer[40];
}USB_PORT_INFO;

static USB_PORT_INFO PORTS[8];


int usb_port_init(USB_PORT_INFO* port_info, bool is_fullSpeed);
USB_PORT_INFO* get_usb_port_info(int index);