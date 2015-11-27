/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module mult #(parameter WIREWIDTH = 1) (input wire clk,
					input wire [WIREWIDTH:0] x,y,
					output reg [WIREWIDTH:0] res); /* FIXME: reswidth! */

   initial begin
      $schematic_boundingbox(100,120);
      $schematic_arc(50,60,40);

      $schematic_linestart;
      $schematic_coord(20,110);
      $schematic_coord(29,94);
      $schematic_lineend;

      $schematic_linestart;
      $schematic_coord(80,110);
      $schematic_coord(71,94);
      $schematic_lineend;

      $schematic_linestart;
      $schematic_coord(50,20);
      $schematic_coord(50,10);
      $schematic_lineend;
      $schematic_linestart;
      $schematic_coord(50,40);
      $schematic_coord(50,80);
      $schematic_lineend;
      $schematic_linestart;
      $schematic_coord(67,70);
      $schematic_coord(33,50);
      $schematic_lineend;
      $schematic_linestart;
      $schematic_coord(67,50);
      $schematic_coord(33,70);
      $schematic_lineend;
      $schematic_connector(x,20,110);
      $schematic_connector(y,80,110);
      $schematic_connector(res,50,10);
   end
   

   always @* begin
      res = x*y;
   end

endmodule
