/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module mux2 #(parameter WIREWIDTH = 1) (input wire s,
					input wire [WIREWIDTH:0] d0, d1,
					output reg [WIREWIDTH:0] o);

   initial begin
      $schematic_boundingbox(40,120);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(30,30);
      $schematic_coord(30,90);
      $schematic_coord(10,110);
      $schematic_polygonend;
      $schematic_linestart;
      $schematic_coord(20,19);
      $schematic_coord(20,10);
      $schematic_lineend;
      $schematic_connector(d0,0,40);
      $schematic_connector(d1,0,80);
      $schematic_connector(o,40,60);
      $schematic_connector(s,20,0);
      $schematic_symboltext("0", 20,40);
      $schematic_symboltext("1", 20,80);
   end
   

   always @* begin
      case(s)
	0: o = d0;
	1: o = d1;
      endcase
   end

endmodule
