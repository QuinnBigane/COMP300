////////
// 
//	comp300-fa21-p2/top.v
//
//	Project 2 Starting Code
//	
//	Second counter and display refresh state machine
//
//	Chuck Pateros
//	University of San Diego
//	06-Oct-2021
//
////////
// look in upduino3.pcf for the mapping of signals to UPduino board pins
module top (
    input CLK,         // 12MHz clock provided by UPduino board
    output [6:0] SEG,  // seven-segment display segments (active low)
    output [3:0] COMM, // seven-segment display common anodes (active high) 
    output [3:0] DBG,  // LEDs installed into breadboard (active high)
    output [2:0] RGB   // RGB LED is built into UPduino board (active low)
);
 
    reg [3:0] hex2Display; // the 4 bits we want to display


    reg [6:0] seg;
    reg [2:0] rgb = 0;
    
    // drive board LEDs

    // Segments 
    assign SEG = seg; // output of case based on hex2Display!
    assign COMM = ~4'b0000;
    assign RGB = ~rgb;

    // Initial hex2Display is just the low order hex digit of second count
    assign hex2Display = second[3:0];

    ////////
    // change simple counter circuit to second counter!
    ////////

    /// second timer signals
    localparam [23:0] timer_init = 24'hB71AFF; // 12 Mhz clock
    reg [23:0] second_timer_state;
    reg [15:0] second;

    /// preload second timer state machine
    initial begin
        second_timer_state = timer_init;
        second = 16'hACD8; // change this to show that hex2Display works!
    end /// end timer state initial begin

    /// second_timer
    /// second timer state machine
    /// increments every second
    always @(posedge CLK) begin
        if(second_timer_state == 0) begin
            second_timer_state = timer_init;
            second++;
        end //end if
        else begin
            second_timer_state = second_timer_state - 1;
        end //end else
    end //end always

    // Use debug LEDs to show second count
    assign DBG = second[3:0];

always @(*) begin
        case(hex2Display) // upside down active low
            4'b0000   : seg = 7'b1000000;
            4'b0001   : seg = 7'b1001111;
            4'b0010   : seg = 7'b0100100;
            4'b0011   : seg = 7'b0000110;
            4'b0100   : seg = 7'b0001011;
            4'b0101   : seg = 7'b0010010;
            4'b0110   : seg = 7'b0010000;
            4'b0111   : seg = 7'b1000111;
            4'b1000   : seg = 7'b0000000;
            4'b1001   : seg = 7'b0000011;
            4'b1010   : seg = 7'b0000001;
            4'b1011   : seg = 7'b0011000;
            4'b1100   : seg = 7'b1110000;
            4'b1101   : seg = 7'b0001100;
            4'b1110   : seg = 7'b0110000;
            4'b1111   : seg = 7'b0110001;       
            default   : seg = 7'b1111111; // go dark in case of error
        endcase // hex to seven-segment case
    end // combinational circuits


endmodule
