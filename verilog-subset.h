/*
    schematic_gui - Lightweight digital schematic capture program
    Copyright (C) 2015 Andreas Ehliar <andreas.ehliar@liu.se>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _VERILOG_SUBSET_H
#define _VERILOG_SUBSET_H
typedef enum {
    IS_IDENTIFIER,
    IS_TILDE, 
    IS_INTEGER,
    IS_MINUS
} constexpression_type;

struct constant_expression{
    constexpression_type type;
    char *ident;
    int integer;
    struct constant_expression *op1;
    struct constant_expression *op2;
};

struct module_parameter {
    struct module_parameter *next;
    const char *name;
    struct constant_expression *expr;
};
typedef enum {UNKNOWN, INPUT, OUTPUT, INOUT} portdirection;

struct module_port{
        struct module_port *next;
        char *name;
        portdirection direction;
        struct constant_expression *highrange;
        struct constant_expression *lowrange;
};

struct wire_def{
  struct wire_def *next;
  char *wirename;
  struct constant_expression *highrange;
  struct constant_expression *lowrange;
};

struct portconnection {
  struct portconnection *next;
  char *name;
  struct constant_expression *connection;
};

struct module_inst{
  struct module_inst *next;
  char *modulename;
  char *instname;
  struct module_parameter *params;
  struct portconnection *ports;
};

typedef enum { SETPOS, SETROT, BOUNDINGBOX, CONNECTOR, POLYGONSTART, LINESTART, ARC, COORD, POLYGONEND, LINEEND, STARTROUTE, ENDROUTE, JUNCTION, FIXEDJUNCTION,POPJUNCTION, SCHEMATICTEXT, SCHEMATICSTARTTEXT, SYMBOLTEXT } schematic_stmt_type;
struct schematic_statement {
        struct schematic_statement *next;
        schematic_stmt_type type;
        char *name1;
        char *name2;
        double x1;
        double y1;
        double x2;
        double y2;
};

// FIXME: Allow $schematic* commands to be evaluated depending on 
struct module{
        const char *modulename;
        struct module_parameter *params;
        struct module_port *ports;
        struct wire_def *wires;
        struct module_inst *insts;
        struct schematic_statement *schematic_info;
};

extern struct module * module_parse_info;
extern int read_instance_info;

void print_module(struct module *m);
void register_verilog_module(struct module *);
#endif
