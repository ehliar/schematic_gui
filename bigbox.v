/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module bigbox #(parameter WIREWIDTH = 1) (input wire s,
					input wire [WIREWIDTH:0] d0, d1,d2,d3,d4,d5,d6,d7,
					output reg [WIREWIDTH:0] o);

   initial begin
      $schematic_boundingbox(380,140);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(10,130);
      $schematic_coord(370,130);
      $schematic_coord(370,10);
      $schematic_polygonend;
      $schematic_connector(d0,0,70);
      $schematic_connector(d1,380,70);
      $schematic_connector(d2,190,0);
      $schematic_connector(d3,190,140);
      $schematic_connector(d4,100,0);
      $schematic_connector(d5,100,140);
      $schematic_connector(d6,280,0);
      $schematic_connector(d7,280,140);
   end
   

endmodule
