/* Module from the schematic_gui program written by Andreas Ehliar <andreas.ehliar@liu.se>
   This Verilog file is licensed under the CC0 license. */
module add #(parameter WIREWIDTH = 1) (input wire ci,
					input wire [WIREWIDTH:0] op1, op2,
					output reg [WIREWIDTH:0] res,
					output reg 		   co);

   initial begin
      $schematic_boundingbox(60,180);
      $schematic_polygonstart;
      $schematic_coord(10,10);
      $schematic_coord(50,50);
      $schematic_coord(50,130);
      $schematic_coord(10,170);
      $schematic_coord(10,95);
      $schematic_coord(15,90);
      $schematic_coord(10,85);
      $schematic_polygonend;
      $schematic_linestart;
      $schematic_coord(30,29);
      $schematic_coord(30,10);
      $schematic_lineend;
      $schematic_linestart;
      $schematic_coord(30,151);
      $schematic_coord(30,170);
      $schematic_lineend;
      $schematic_connector(op1,0,50);
      $schematic_connector(op2,0,130);
      $schematic_connector(res,60,90);
      $schematic_connector(ci,30,0);
      $schematic_connector(co,30,180);
      
   end

   always @* begin
      {co,res} = {1'b0,op1} + {1'b0,op2} + ci;
   end
   
endmodule
