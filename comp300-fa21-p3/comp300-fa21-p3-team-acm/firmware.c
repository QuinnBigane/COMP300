////////
// 
//	comp300-fa21-p3/firmware.c
//
//	Project 3 Starter Code
//	
//	SOFTWARE multiplexed 7-segment display 
//  on the breadboard using this C program on a 
//  PICOSOC RISC-V processor instantiated on the FPGA.
//
//	Koby, Quinn, Andres, Claire
//	University of San Diego
//	24-Oct-2021
//
////////

#include <stdint.h>
#include <stdbool.h>


// a pointer to this is a null pointer, but the compiler does not
// know that because "sram" is a linker symbol from sections.lds.
extern uint32_t sram;

#define reg_spictrl (*(volatile uint32_t*)0x02000000)
#define reg_uart_clkdiv (*(volatile uint32_t*)0x02000004)
#define reg_uart_data (*(volatile uint32_t*)0x02000008)
#define reg_gpio (*(volatile uint32_t*)0x03000000)

extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss,_heap_start;

uint32_t set_irq_mask(uint32_t mask); asm (
    ".global set_irq_mask\n"
    "set_irq_mask:\n"
    ".word 0x0605650b\n"
    "ret\n"
);

void spin_wait_ms(int wait_ms){
    int cycles_to_wait = 25 * wait_ms;
    while (cycles_to_wait >= 0)
      cycles_to_wait -= 1;
  }

uint32_t segment_decode(uint32_t hex_to_display){
     switch(hex_to_display){ // upside down
            case 0b0000 : return 0b1000000;
            case 0b0001 : return 0b1111001;
            case 0b0010 : return 0b0100100;
            case 0b0011 : return 0b0110000;
            case 0b0100 : return 0b0011001;
            case 0b0101 : return 0b0010010;
            case 0b0110 : return 0b0000010;
            case 0b0111 : return 0b1111000;
            case 0b1000 : return 0b0000000;
            case 0b1001 : return 0b0011000;
            case 0b1010 : return 0b0001000;
            case 0b1011 : return 0b0000011;
            case 0b1100 : return 0b1000110;
            case 0b1101 : return 0b0100001;
            case 0b1110 : return 0b0000110;
            case 0b1111 : return 0b0001110;
            default   : return ~0b0000000; // go dark in case of error
        }
}

void main() {
    set_irq_mask(0xff);

    // zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }

    // switch to dual IO mode
    reg_spictrl = (reg_spictrl & ~0x007F0000) | 0x00400000;

    uint32_t led_timer = 0;
    uint32_t second_timer = 0;
    uint32_t ms_timer = 0;
    uint32_t display_digit = 0;
    uint32_t comm = 0b1111;
    uint32_t segments = 0b1000001;
    uint32_t hex_to_display = 0x0;

    while (1) {
      spin_wait_ms(1);
      ms_timer += 1;
// hint for second lab
      segments = segment_decode(hex_to_display);

// Tie outputs to GPIO lines (in Verilog top.v)
//    assign DBG = gpio[20:17];

      reg_gpio =  ((second_timer & 0x00F) << 17) | 
                  ((comm & 0x00F) << 21) |
                  ((segments & 0x07F) << 25); // debug LEDs

        // do display updates here
        switch(display_digit) {
            case 0 : {
              display_digit = 3;
              comm = ~0b0111;
              hex_to_display = second_timer & 0x0F;
              break;
            }
            case 1 : {
              display_digit = 2;
              comm = ~0b1011;
              hex_to_display = (second_timer >> 4) & 0x0F;
              break;
            }
            case 2 : {
              display_digit = 0;
              comm = ~0b1101;
              hex_to_display = (second_timer >> 8) & 0x0F;
              break;
            }
            case 3 : {
              display_digit = 1;
              comm = ~0b1110;
              hex_to_display = (second_timer >> 12) & 0x0F;
              break;
          }
            default : {
              display_digit = 0;
              comm = ~0b0000;
              break;
          }
        } // end switch

      if(ms_timer > 999){
      // get here once a second
      ms_timer = 0;
      second_timer += 1;
    } // end of if
  } // end of while(1)
} // end of main program
