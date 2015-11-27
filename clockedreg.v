/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module clockedreg #(parameter WIREWIDTH = 1) (input wire clk,
					input wire [WIREWIDTH:0] d,
					output reg [WIREWIDTH:0] q);

   initial begin
      $schematic_boundingbox(60,120);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(50,10);
      $schematic_coord(50,110);
      $schematic_coord(10,110);
      $schematic_polygonend;
      $schematic_linestart;
      $schematic_coord(10,110);
      $schematic_coord(30,90);
      $schematic_coord(50,110);
      $schematic_lineend;
      $schematic_connector(d,0,60);
      $schematic_connector(q,60,60);
   end
   

   always @(posedge clk) begin
      q <= d;
   end

endmodule
