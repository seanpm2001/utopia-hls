module W7(

    clock,
    reset,
    value);

output [15:0] value;
input clock;
input reset;
wire [15:0] value;
assign value = 565;
endmodule //W7
