////////
// 
//	comp300-fa21-p2-1/top.v
//
//	Project 2 Second Week Starter
//	
//	This circuit creates a a multiplexed 7-segment display
//	using the 12 Mhz clock of the Upduino3 board. The
//      display is tested with a counter that counts seconds. 
//
//	Chuck Pateros
//	University of San Diego
//	13-Oct-2021
//
////////


// look in upduino3.pcf for the mapping of signals to UPduino board pins
module top (
    input CLK,         // 12MHz clock provided by UPduino board
    output [6:0] SEG,  // seven-segment display segments (active high)
    output [3:0] COMM, // seven-segment display common cathodes (active low) 
    output [3:0] DBG,  // LEDs installed into breadboard (active high)
    output [2:0] RGB   // RGB LED is built into UPduino board (active low)
);
 
    reg [3:0] hex2Display; // the 4 bits we want to display

 
    reg [6:0] seg;
    reg [3:0] comm;
   
    // drive board LEDs
    assign SEG = ~seg;
    assign RGB = 3'b111;
    assign COMM = ~comm;

    /// second timer signals
    localparam [23:0] timer_init = 24'hB71AFF;
    reg [23:0] second_timer_state;
    reg [15:0] displayDigit;

    /// preload second timer state machine
    initial begin
        second_timer_state = timer_init;
        displayDigit = 16'hACDC;
    end /// end timer state initial begin

    /// second_timer
    /// second timer state machine
    /// increments every second
    always @(posedge CLK) begin
        if (second_timer_state == 0) begin
            second_timer_state = timer_init;
            displayDigit = displayDigit + 1;
         end /// end if
        else begin
            second_timer_state <= second_timer_state - 1;
        end /// end else
    end /// second timer state machine

    // Use debug LEDs to show low order 4 bits of second count
    assign DBG = displayDigit[3:0];


    ////////
    // simple refreshCounter circuit 
    // TODO: Replace with your digit scrolling state machine!
    ////////

    // keep track of refreshCount
    reg [23:0] refreshCounter;

 
    // increment the refreshCounter every clock
    always @(posedge CLK) begin
        refreshCounter <= refreshCounter + 1;
    end


    always @(*) begin
        case(hex2Display) // upside down
            4'b0000   : seg = 7'b0111111;
            4'b0001   : seg = 7'b0110000;
            4'b0010   : seg = 7'b1011011;
            4'b0011   : seg = 7'b1111001;
            4'b0100   : seg = 7'b1110100;
            4'b0101   : seg = 7'b1101101;
            4'b0110   : seg = 7'b1101111;
            4'b0111   : seg = 7'b0111000;
            4'b1000   : seg = 7'b1111111;
            4'b1001   : seg = 7'b1111100;
            4'b1010   : seg = 7'b1111110;
            4'b1011   : seg = 7'b1100111;
            4'b1100   : seg = 7'b0001111;
            4'b1101   : seg = 7'b1110011;
            4'b1110   : seg = 7'b1001111;
            4'b1111   : seg = 7'b1001110;
           // default   : seg = 7'b0000000; // go dark in case of error
        endcase // hex to seven-segment case
         
        case(refreshCounter[22:21]) // digit scrolling 0,1,2,3 (Pateros)
            4'b00   : comm = 4'b1110;
            4'b01   : comm = 4'b1101;
            4'b11   : comm = 4'b1011;
            4'b10   : comm = 4'b0111;
            default : comm = 4'b0000; // light all if error
       endcase

    /// selects the hex digit for the currently displayed digit
        case(refreshCounter[22:21])
            4'b00   : hex2Display = displayDigit[3:0];
            4'b01   : hex2Display = displayDigit[7:4];
            4'b11   : hex2Display = displayDigit[11:8];
            4'b10   : hex2Display = displayDigit[15:12];
            default : hex2Display = displayDigit[3:0];  // 'E' for error!  
        endcase
       
    end // combinational circuits

endmodule

