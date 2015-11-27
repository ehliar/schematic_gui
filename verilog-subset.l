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
%{
#include <stdio.h>
#include "verilog-subset.h"
#include "verilog-subset.tab.h"
int start_token;
%}
%option noyywrap
%option nounput
%option header-file="verilog-subset.yy.h"
%x INCOMMENT
%%
%{
        if(start_token){
                int t = start_token;
                start_token = 0;
                return t;
        }
%}


<INITIAL>{
"/*"            {  BEGIN(INCOMMENT);}
[ \t]+            // Ignore
\n                // Ignore
module yylval.sval = strdup(yytext); return TOK_MODULE;
parameter return TOK_PARAMETER;
#       return TOK_HASH;
\(      return TOK_LEFTPAREN;
\)      return TOK_RIGHTPAREN;
=       return TOK_EQUAL;
~       return TOK_TILDE;
"."     return TOK_POINT;
input   return TOK_INPUT;
output  return TOK_OUTPUT;
,       return TOK_COMMA;
:       return TOK_COLON;
;       return TOK_SEMICOLON;
\[      return TOK_LEFTBRACKET;
\]      return TOK_RIGHTBRACKET;
wire    return TOK_WIRE;
reg     return TOK_REG;
initial return TOK_INITIAL;
begin   return TOK_BEGIN;
end     return TOK_END;
endmodule return TOK_ENDMODULE;
$schematic_startroute return TOK_STARTROUTE;
$schematic_endroute   return TOK_ENDROUTE;
$schematic_setgatepos return TOK_SETGATEPOS;
$schematic_setgaterot return TOK_SETGATEROT;
$schematic_boundingbox return TOK_BOUNDINGBOX;
$schematic_connector return TOK_CONNECTOR;
$schematic_polygonstart return TOK_POLYGONSTART;
$schematic_linestart return TOK_LINESTART;
$schematic_arc return TOK_ARC;
$schematic_coord return TOK_COORD;
$schematic_polygonend return TOK_POLYGONEND;
$schematic_lineend return TOK_LINEEND;
$schematic_junction return TOK_JUNCTION;
$schematic_symboltext return TOK_SYMBOLTEXT;
$schematic_text return TOK_TEXT;
$schematic_starttext return TOK_STARTTEXT;
$schematic_popjunction return TOK_POPJUNCTION;
$schematic_fixed_junction return TOK_FIXED_JUNCTION;
(-)?[0-9]+ yylval.ival = atoi(yytext); return TOK_INTEGER;
-       return TOK_MINUS;
[a-zA-Z_$][a-zA-Z_$0-9]* yylval.sval = strdup(yytext); return TOK_IDENTIFIER;
\"(\\.|[^"])*\" yylval.sval = strdup(&yytext[1]); yylval.sval[strlen(yylval.sval)-1]=0; return TOK_STRING;
}


<INCOMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        yylineno++;
}




