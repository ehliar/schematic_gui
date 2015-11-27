module test;
    wire unnamedwire5;
    wire unnamedwire6;
    wire unnamedwire7;
    wire unnamedwire8;
    wire unnamedwire9;
    wire unnamedwire10;
    wire unnamedwire21;
    wire unnamedwire22;
    wire unnamed_wire0;
    wire unnamedwire20;
    wire unnamedwire17;
    wire unnamedwire19;
    wire unnamedwire12;
    wire unnamedwire13;
    wire unnamedwire18;
    wire unnamedwire16;
    wire unnamedwire11;
    wire unnamedwire15;
    wire unnamedwire14;

    mult inst_07(.x(unnamedwire19),
        .y(unnamed_wire0),
        .res(unnamedwire20));

    clockedreg inst_02(.d(unnamedwire7),
        .q(unnamedwire10));

    mux2 inst_03(.d0(unnamedwire9),
        .d1(unnamedwire13),
        .o(unnamedwire5),
        .s(unnamedwire6));

    mux2 inst_04(.d0(unnamedwire10),
        .d1(unnamedwire14),
        .o(unnamedwire7),
        .s(unnamedwire8));

    add inst_00(.op1(unnamedwire21),
        .op2(unnamedwire15),
        .res(unnamedwire22));

    box inst_08(.d2(unnamedwire20),
        .d3(unnamedwire21));

    mux2 inst_06(.d0(unnamedwire16),
        .d1(unnamedwire17),
        .o(unnamedwire15),
        .s(unnamedwire18));

    mux2 inst_05(.d0(unnamedwire10),
        .d1(unnamedwire9),
        .o(unnamedwire12),
        .s(unnamedwire11));

    clockedreg inst_01(.d(unnamedwire5),
        .q(unnamedwire9));

    initial begin
        $schematic_setgatepos(inst_07, 940,190);
        $schematic_setgaterot(inst_07, 3);
        $schematic_setgatepos(inst_02, 410,280);
        $schematic_setgaterot(inst_02, 3);
        $schematic_setgatepos(inst_03, 130,210);
        $schematic_setgaterot(inst_03, 3);
        $schematic_starttext("GUARD", 1130, 300, 0);
        $schematic_setgatepos(inst_04, 410,210);
        $schematic_setgaterot(inst_04, 3);
        $schematic_setgatepos(inst_00, 1240,310);
        $schematic_setgatepos(inst_08, 1060,270);
        $schematic_setgatepos(inst_06, 1040,380);
        $schematic_starttext("ACR0", 200, 310, 0);
        $schematic_setgatepos(inst_05, 340,440);
        $schematic_setgaterot(inst_05, 3);
        $schematic_setgatepos(inst_01, 130,280);
        $schematic_setgaterot(inst_01, 3);
        $schematic_starttext("ACR1", 480, 310, 0);
        $schematic_startroute(inst_07.y);
        $schematic_text("OpB", 890, 270, 2);
        $schematic_startroute(inst_00.res);
        $schematic_text("RES", 1340, 400, 1);
        $schematic_startroute(inst_08.d3);
        $schematic_endroute(inst_00.op1);
        $schematic_startroute(inst_07.res);
        $schematic_endroute(inst_08.d2);
        $schematic_startroute(inst_07.x);
        $schematic_text("OpA", 890, 210, 2);
        $schematic_startroute(inst_06.s);
        $schematic_text("Copa", 1060, 350, 3);
        $schematic_startroute(inst_06.d1);
        $schematic_text("ACR", 980, 460, 2);
        $schematic_startroute(inst_06.d0);
        $schematic_text("0", 990, 420, 2);
        $schematic_startroute(inst_06.o);
        $schematic_endroute(inst_00.op2);
        $schematic_startroute(inst_04.d1);
        $schematic_text("RES", 450, 150, 3);
        $schematic_startroute(inst_03.d1);
        $schematic_text("RES", 170, 150, 3);
        $schematic_startroute(inst_05.o);
        $schematic_text("ACR", 400, 510, 4);
        $schematic_startroute(inst_05.s);
        $schematic_text("Csel", 490, 460, 1);
        $schematic_startroute(inst_02.q);
        $schematic_junction(470, 370);
        $schematic_endroute(inst_05.d0);
        $schematic_fixed_junction(620, 370);
        $schematic_fixed_junction(620, 190);
        $schematic_endroute(inst_04.d0);
        $schematic_popjunction;
        $schematic_popjunction;
        $schematic_popjunction;
        $schematic_startroute(inst_01.q);
        $schematic_junction(330, 369);
        $schematic_endroute(inst_05.d1);
        $schematic_fixed_junction(330, 370);
        $schematic_fixed_junction(330, 190);
        $schematic_endroute(inst_03.d0);
        $schematic_popjunction;
        $schematic_popjunction;
        $schematic_popjunction;
        $schematic_startroute(inst_04.s);
        $schematic_text("Cw1", 570, 230, 1);
        $schematic_startroute(inst_04.o);
        $schematic_endroute(inst_02.d);
        $schematic_startroute(inst_03.s);
        $schematic_text("Cw0", 290, 230, 1);
        $schematic_startroute(inst_03.o);
        $schematic_endroute(inst_01.d);
    end
endmodule
