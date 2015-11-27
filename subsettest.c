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


// Test program for the parser
#include <stdio.h>
#include "verilog-subset.h"
#include "verilog-subset.tab.h"
extern int start_token;
int main(int argc, char **argv)
{
        if(argc > 1){
                yydebug = 1;
        }

	start_token = START_WITH_LIMITED_PARSER;

        if(argc > 1){
	  read_instance_info = 1;
	}
        yyparse();
        print_module(module_parse_info);
}
