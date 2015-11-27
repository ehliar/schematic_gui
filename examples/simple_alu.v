module test;
    wire unnamed_wire1;
    wire unnamed_wire2;
    wire unnamed_wire5;
    wire unnamed_wire4;
    wire unnamed_wire0;
    wire unnamed_wire3;

    add inst_00(.op1(unnamed_wire1),
        .op2(unnamed_wire4),
        .res(unnamed_wire0),
        .ci(unnamed_wire2));

    clockedreg inst_01(.d(unnamed_wire0),
        .q(unnamed_wire5));

    mux2 inst_02(.d0(unnamed_wire3),
        .d1(~unnamed_wire3),
        .o(unnamed_wire1),
        .s(unnamed_wire2));

    initial begin
        $schematic_setgatepos(inst_00, 390,270);
        $schematic_setgatepos(inst_01, 520,300);
        $schematic_setgatepos(inst_02, 320,260);
        $schematic_startroute(inst_01.q);
        $schematic_text("Result", 670, 360, 1);
        $schematic_startroute(inst_00.op2);
        $schematic_text("OpB", 200, 400, 2);
        $schematic_startroute(inst_02.d1);
        $schematic_fixed_junction(290, 320);
        $schematic_junction(289, 300);
        $schematic_endroute(inst_02.d0);
        $schematic_text("OpA", 200, 300, 2);
        $schematic_popjunction;
        $schematic_popjunction;
        $schematic_startroute(inst_00.ci);
        $schematic_junction(340, 260);
        $schematic_text("Csub", 340, 180, 3);
        $schematic_endroute(inst_02.s);
        $schematic_popjunction;
        $schematic_startroute(inst_02.o);
        $schematic_endroute(inst_00.op1);
        $schematic_startroute(inst_00.res);
        $schematic_endroute(inst_01.d);
    end
endmodule
