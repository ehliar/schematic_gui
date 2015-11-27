/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module mux3 #(parameter WIREWIDTH = 1) (input wire [1:0] s,
					input wire [WIREWIDTH:0] d0, d1, d2,
					output reg [WIREWIDTH:0] o);

   initial begin
      $schematic_boundingbox(40,160);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(30,30);
      $schematic_coord(30,130);
      $schematic_coord(10,150);
      $schematic_polygonend;
      $schematic_linestart;
      $schematic_coord(20,19);
      $schematic_coord(20,10);
      $schematic_lineend;
      $schematic_connector(d0,0,40);
      $schematic_connector(d1,0,80);
      $schematic_connector(d2,0,120);
      $schematic_connector(o,40,80);
      $schematic_connector(s,20,0);
      $schematic_symboltext("0", 20,40);
      $schematic_symboltext("1", 20,80);
      $schematic_symboltext("2", 20,120);
   end
   

   always @* begin
      case(s)
	0: o = d0;
	1: o = d1;
        default: o = d2;
      endcase
   end

endmodule
