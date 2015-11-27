/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module box #(parameter WIREWIDTH = 1) (input wire s,
					input wire [WIREWIDTH:0] d0, d1,d2,d3,d4,d5,d6,d7,
					output reg [WIREWIDTH:0] o);

   initial begin
      $schematic_boundingbox(140,60);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(10,50);
      $schematic_coord(130,50);
      $schematic_coord(130,10);
      $schematic_polygonend;
      $schematic_connector(d0,0,30);
      $schematic_connector(d1,140,30);
      $schematic_connector(d2,70,0);
      $schematic_connector(d3,70,60);
      $schematic_connector(d4,40,0);
      $schematic_connector(d5,40,60);
      $schematic_connector(d6,100,0);
      $schematic_connector(d7,100,60);
   end
   

endmodule
