////////
// 
//	p4/firmware.c
//
//	Project 5 
//	
//	Hardware multiplexed 7-segment display 
//  on the breadboard using this C program on a 
//  PICOSOC RISC-V processor instantiated on the FPGA.
//
//  The second count is provided as a 16 bit binary number
//  in 4 hex digits.  Note that 4 decimal digits can be
//  sent to the display by sending 4 BCD digits instead of hex.
//
//  reg_gpio = ...      //GPIO output to display circuit
//  var = reg_gpio ...  // input from display circuit GPIO to program
//
//	Team ACM (Koby, Claire, Quinn, Andres)
//	University of San Diego
//	1-Dec-2021
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

// Here is a function if you don't have "/" and "%"
uint32_t convert(uint32_t num)
{
    unsigned char min_ten = 0;
    unsigned char min_one = 0;
    unsigned char sec_ten = 0;
    unsigned char sec_one = 0;
    while(num >= 600)
    {
        min_ten = min_ten + 1;
        num = num - 600;
    }
    while(num >= 60)
    {
        min_one = min_one + 1;
        num = num - 60;
    }
    while(num >= 10)
    {
        sec_ten = sec_ten + 1;
        num = num - 10;
    }
    sec_one = num;
    return (min_ten << 12) | (min_one << 8) | (sec_ten << 4) | sec_one;
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
    uint32_t second_timer = 86390;
    uint32_t ms_timer = 0;
    uint32_t display_digit = 0;
    uint32_t comm = 0b1110;
    uint32_t segments = 0b1111111;
    uint32_t hex_to_display = 0x0;
    uint32_t second_toggle = 0;
    uint32_t new_second_toggle = 0;
    uint32_t display = 0;
    uint32_t minutes = 0;
    uint32_t min_tens = 0;
    uint32_t min_ones = 0;
    uint32_t hour_tens = 0;
    uint32_t hour_ones = 0;
    uint32_t hours = 0;
    bool dec = 1;
    
    bool set_mode = false;
    bool reset_mode = false;
    bool increment_hours = false;
    bool increment_minutes = false;
    while (1) {

        // read values from hardware
        //      assign read_data = ((second_toggle & 32'b1)); // from top.v
        new_second_toggle = reg_gpio & 0x1;


        while(new_second_toggle == second_toggle){
            new_second_toggle = reg_gpio & 0x1;
        }
        second_toggle = new_second_toggle;
      
        //If in set mode, read increment hrs
        if((reg_gpio&0b100) && (reg_gpio&0b010)){
            second_timer+=3600;
        }
        //if in set mode, read increment minutes
        if((reg_gpio&0b1000) && (reg_gpio&0b010)){
            second_timer+=60;
        }
        
        //Read Reset Mode 
        if(reg_gpio&0b10000)
        {
            if(second_timer % 3600 >= 1800){
                second_timer = ((second_timer / 3600) * 3600) + 3600;

            }
            else{
                second_timer = (second_timer / 3600) * 3600;
            }
        }

        if(!(reg_gpio&0b10))
        {
            second_timer++;
        }

       
        if(second_timer > 86399)
        {
            second_timer = 0;
        }

    // Following code works when "/" and "%" implemented
      
    hours = second_timer / 3600;
    hour_tens = hours / 10;
    hour_ones = hours %10;
    minutes = second_timer % 3600;
    minutes = minutes / 60;
    min_tens = minutes / 10;
    min_ones = minutes % 10;
  
      reg_gpio = (hour_tens << 12) | (hour_ones << 8) | (min_tens << 4) | min_ones;

    // End of "/" and "%" code
    
    // Code to use a function if "/" and "%" not implemented
    // uint32_t con = convert(second_timer);
    //reg_gpio = con; // debug LEDs in 4 LSBs
       
  } // end of while(1)
} // end of main program

