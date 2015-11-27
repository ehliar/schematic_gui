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
#include <stdlib.h>
#include <string.h>
#include "verilog-subset.h"
#include "verilog-subset.tab.h"

int read_instance_info = 0;

struct module * module_parse_info;

struct constant_expression *constexpr_identifier(char *identifier)
{
	struct constant_expression *tmp = (struct constant_expression *) malloc(sizeof(struct constant_expression));
    if(!tmp) abort();
    tmp->type = IS_IDENTIFIER;
    tmp->ident = identifier;
        return tmp;
}

struct constant_expression *constexpr_integer(int x)
{
	struct constant_expression *tmp = (struct constant_expression *) malloc(sizeof(struct constant_expression));
    if(!tmp) abort();
    tmp->type = IS_INTEGER;
    tmp->integer = x;
        return tmp;
    
}

// Ok, this is ugly, but it will get better once we C++:ify the program
char constexpr_buf[1024];
char *constexpr_to_string(struct constant_expression *s, FILE *memfp)
{
        if(!memfp){
                memfp = fmemopen(constexpr_buf, 1000, "w");
                constexpr_to_string(s,memfp);
                fclose(memfp);
                return &constexpr_buf[0];
        }

        switch(s->type){
        case IS_IDENTIFIER:
                fprintf(memfp, "%s", s->ident);
                break;
        case IS_INTEGER:
                fprintf(memfp, "%d", s->integer);
                break;
	case IS_TILDE:
                fprintf(memfp, "~");
		constexpr_to_string(s->op1, memfp);
		break;
        case IS_MINUS:
                constexpr_to_string(s->op1, memfp);
                fprintf(memfp," - ");
                constexpr_to_string(s->op2, memfp);
                break;
        default:
                fprintf(stderr,"Internal error in constexpr_to_string\n");
                abort();
        }
	return NULL;
}

struct constant_expression *constexpr_treenode(constexpression_type optype, struct constant_expression *op1, struct constant_expression *op2)
{
	struct constant_expression *tmp = (struct constant_expression *) malloc(sizeof(struct constant_expression));
    if(!tmp) abort();
    tmp->type = optype;
    tmp->op1 = op1;
    tmp->op2 = op2;
        return tmp;
    
}


void print_module_parameters(struct module_parameter *m)
{
	while(m){
		printf("        %s  = %s\n", m->name, constexpr_to_string(m->expr, NULL));
		m=m->next;
	}
}
struct module_parameter *newmodparam(const char *name, struct constant_expression *val)
{
        struct module_parameter *tmp = (struct module_parameter *) malloc(sizeof(struct module_parameter));
        if(!tmp) abort();

        tmp->next = NULL;
        tmp->name = name;
        tmp->expr = val;
        return tmp;
        
}

struct portconnection *create_new_portconn(char *name, struct constant_expression *signal)
{
	struct portconnection *tmp = (struct portconnection *) malloc(sizeof(struct portconnection));
	if(!tmp) abort();

	tmp->next = NULL;
	tmp->name = name;
	tmp->connection = signal;
        return tmp;

}

struct wire_def *create_new_wire(char *name, struct constant_expression *highrange, struct constant_expression *lowrange)
{
	struct wire_def *tmp = (struct wire_def *) malloc(sizeof(struct wire_def));
	if(!tmp) abort();

	tmp->next = NULL;
	tmp->wirename = name;
	tmp->highrange = highrange;
	tmp->lowrange = lowrange;
        return tmp;
}

void print_wire_defs(struct wire_def *w)
{
	while(w){
		if(w->highrange){
			printf("        %s [ %s :", w->wirename, constexpr_to_string(w->highrange,NULL));
			printf(" %s ]\n", constexpr_to_string(w->lowrange, NULL));
		}else{
			printf("        %s\n", w->wirename);
		}
		w=w->next;
	}
}

struct module_inst *create_new_module_inst(char *modname, char *instname, struct module_parameter *params, struct portconnection *ports)
{
	struct module_inst *tmp = (struct module_inst *) malloc(sizeof(struct module_inst));
	if(!tmp) abort();

	tmp->next = NULL;
	tmp->modulename = modname;
	tmp->instname = instname;
	tmp->params = params;
	tmp->ports = ports;
        return tmp;
        
}

struct module *add_wire_to_module(struct module *m, struct wire_def *w)
{
	if(!m){
		m = (struct module *) malloc(sizeof(struct module));
		if(!m) abort();
		m->modulename = 0;
		m->params = 0;
		m->ports = 0;
		m->wires = 0;
		m->insts = 0;
                m->schematic_info = NULL;
	}
	struct wire_def *tmp = m->wires;
	if(m->wires){
		tmp = m->wires;
		while(tmp->next){
			tmp = tmp->next;
		}
		tmp->next = w;
	}else{
		m->wires = w;
	}
	return m;
}

struct module *add_moduleinst_to_module(struct module *m, struct module_inst *inst)
{
	if(!m){
		m = (struct module *) malloc(sizeof(struct module));
		if(!m) abort();
		m->modulename = 0;
		m->params = 0;
		m->ports = 0;
		m->wires = 0;
		m->insts = 0;
                m->schematic_info = NULL;
	}
	struct module_inst *tmp = m->insts;
	if(m->insts){
		tmp = m->insts;
		while(tmp->next){
			tmp = tmp->next;
		}
		tmp->next = inst;
	}else{
		m->insts = inst;
	}
	return m;
}


struct module_port *create_new_port(char *name, portdirection dir, struct constant_expression *high,
                                    struct constant_expression *low)
{
        struct module_port *tmp;
        tmp = (struct module_port *) malloc(sizeof(struct module_port));
        if(!tmp) abort();

        tmp->next = NULL;
        tmp->name = name;
        tmp->direction = dir;
        tmp->highrange = high;
        tmp->lowrange = low;
        return tmp;
}

struct schematic_statement *create_schematic_statement(schematic_stmt_type type, char *name1, char *name2, double x1, double y1, double x2, double y2)
{
        struct schematic_statement *tmp = (struct schematic_statement *) malloc(sizeof(struct schematic_statement));
        if(!tmp) abort();
	tmp->next = NULL;

        tmp->type = type;
        tmp->name1 = name1;
        tmp->name2 = name2;
        tmp->x1 = x1;
        tmp->y1 = y1;
        tmp->x2 = x2;
        tmp->y2 = y2;
        return tmp;
}

void module_port_print(struct module_port *p)
{
        printf("    Module ports are:\n");
	if(!p){
	  printf("EMPTY?\n");
	}
        while(p){
		printf("        ");
                switch(p->direction){
                case INPUT: printf("input: "); break;
                case OUTPUT: printf("output: "); break;
                default:
                        abort();
                }
                printf("   %s  ", p->name);
                if(p->highrange){
                        printf(" [%s:", constexpr_to_string(p->highrange,NULL));
                        printf("%s]", constexpr_to_string(p->lowrange,NULL));
                }
                printf("\n");
                p=p->next;
        }
}

void print_portconns(struct portconnection *ports)
{
	while(ports){
		printf("            %s <- %s\n", ports->name, constexpr_to_string(ports->connection,NULL));
		ports = ports->next;
	}
}
void print_module_insts(struct module_inst *inst)
{
	while(inst){
		printf("        Module name: %s\n", inst->modulename);
		printf("        Instance name: %s\n", inst->instname);
		printf("        Module parameters:\n");
		print_module_parameters(inst->params);
		print_portconns(inst->ports);
		inst = inst->next;
	}
}

void print_schematic_statements(struct schematic_statement *s)
{
        while(s){
                switch(s->type){
                case SETPOS:
                        printf("        SETPOS(%s, %f, %f\n", s->name1, s->x1, s->y1);
                        break;
                default:
                        // fprintf(stderr,"Unknown schematic statement %d\n", s->type);
			break;
                }
                s=s->next;
        }
}
void print_module(struct module *m)
{
	printf("MODULE NAME IS %s\n", m->modulename);
	printf("    Module parameters are: \n");
	print_module_parameters(m->params);
	printf("    Module inputs/outputs are: \n");
	module_port_print(m->ports);
	printf("    Module wires are: \n");
	print_wire_defs(m->wires);

	printf("    Module instantiations are: \n");
	print_module_insts(m->insts);

        printf("    Schematic statements are: \n");
        print_schematic_statements(m->schematic_info);
}

void yyerror(const char *str)
{
  fprintf(stderr,"error: %s\n",str);
}
int yywrap()
{
	return 1;
}

 int yyparse(void);
 int yylex(void);
//  int main(int argc, char **argv)
//  {
//    yyparse();
//  }

%}

%union {
	int ival;
	char *sval;
	portdirection direction;
	struct module_parameter *modparam;
	struct module_port *port;
	struct constant_expression * constant_expr;
	struct portconnection *portconn;
	struct module_inst *inst;
	struct wire_def *wiredef;
	struct module *mod;
        struct schematic_statement *stmt;
}

%token <ival> TOK_INTEGER
%token <sval> TOK_IDENTIFIER
%token <sval> TOK_STRING

%token TOK_MODULE TOK_PARAMETER TOK_HASH TOK_LEFTPAREN TOK_RIGHTPAREN TOK_EQUAL TOK_INPUT TOK_OUTPUT TOK_COMMA TOK_COLON TOK_LEFTBRACKET TOK_RIGHTBRACKET TOK_MINUS TOK_ENDMODULE  TOK_SEMICOLON TOK_WIRE TOK_REG TOK_FULCOMMA TOK_POINT TOK_INITIAL TOK_BEGIN TOK_END TOK_SETGATEPOS TOK_SETGATEROT TOK_BOUNDINGBOX TOK_CONNECTOR TOK_POLYGONSTART TOK_LINESTART TOK_ARC TOK_COORD TOK_POLYGONEND TOK_LINEEND TOK_STARTROUTE TOK_ENDROUTE TOK_JUNCTION TOK_FIXED_JUNCTION START_WITH_LIMITED_PARSER START_WITH_SUBMODULE_PARSER TOK_POPJUNCTION TOK_TEXT TOK_STARTTEXT TOK_SYMBOLTEXT TOK_TILDE



%type <constant_expr> constexpression
%type <modparam>         parameter_inst parameter_assign parameter_insts
%type <port>            inputoutputs inputoutputstatements bitwidthstatement signallist
%type   <direction>      directionstatement
%type <modparam> parameterdef parameters parameter
%type <portconn> signal_insts
%type <wiredef> wiredef
%type <inst> moduleinst
%type <mod> module_content modulestmt module module_and_initial
%type <stmt> schematic_stmts initial_block schematic_stmt
%%

start: START_WITH_LIMITED_PARSER module
       |
       START_WITH_SUBMODULE_PARSER module_and_initial
       ;



module: modulestmt module_content TOK_ENDMODULE
        {
		$$ = $1;
		$$->insts = $2->insts;
		$$->wires = $2->wires;
                $$->schematic_info = $2->schematic_info;
		free($2);
		// print_module($$);
		module_parse_info = $$;
	}
        ; 



module_and_initial: modulestmt initial_block
        {
		$$ = $1;
                $$->schematic_info = $2;
		// print_module($$);
		module_parse_info = $$;
		printf("Finished module_and_initial!\n");
		YYACCEPT;
	}
        ; 

module_content:
        module_content wiredef { $$ = add_wire_to_module($1, $2); }
        |
        module_content moduleinst { $$ = add_moduleinst_to_module($1,$2); }
        |
        module_content initial_block {
                if($$->schematic_info){
                        fprintf(stderr,"Error: Multiple initial statements are not supported at this time.\n");
                        YYERROR;
                }
                $$ = $1;
                $$->schematic_info = $2;
        }
        |
        initial_block {
                struct module *tmp = (struct module *) malloc(sizeof(struct module));
                if(!tmp) abort();

                tmp->modulename = NULL;
                tmp->params = NULL;
                tmp->ports = NULL;
                tmp->wires = NULL;
                tmp->insts = NULL;
                tmp->schematic_info = $1;
                $$=tmp;
		if(read_instance_info){
			module_parse_info = $$;
			YYACCEPT;
		}
        }
        |
        wiredef { $$ = add_wire_to_module(NULL, $1); }
        |
        moduleinst { $$ = add_moduleinst_to_module(NULL,$1); }
        ;

wiredef:
                TOK_WIRE bitwidthstatement TOK_IDENTIFIER TOK_SEMICOLON
                {
			if($2){
				$$ = create_new_wire($3, $2->highrange, $2->lowrange);
				free($2);
			}else{
				$$ = create_new_wire($3, NULL,NULL);
			}
                }
        ;



moduleinst:        TOK_IDENTIFIER parameter_insts TOK_IDENTIFIER TOK_LEFTPAREN signal_insts TOK_RIGHTPAREN TOK_SEMICOLON
        {
		$$ = create_new_module_inst($1, $3, $2, $5);
        }
        | TOK_IDENTIFIER parameter_insts TOK_IDENTIFIER TOK_LEFTPAREN TOK_RIGHTPAREN TOK_SEMICOLON
        {
		$$ = create_new_module_inst($1, $3, $2, NULL);
        }
        ;


initial_block: TOK_INITIAL TOK_BEGIN schematic_stmts TOK_END {
	                $$ = $3;
		};

schematic_stmt: TOK_BOUNDINGBOX TOK_LEFTPAREN TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON {
	$$ = create_schematic_statement(BOUNDINGBOX, NULL,NULL,$3,$5,0,0);
		}
                |
                TOK_CONNECTOR TOK_LEFTPAREN TOK_IDENTIFIER TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON {
			$$ = create_schematic_statement(CONNECTOR, $3,NULL,$5,$7,0,0);
		}
                |
                TOK_POLYGONSTART  TOK_SEMICOLON {
			$$ = create_schematic_statement(POLYGONSTART, NULL,NULL,0,0,0,0);
		}
                |
                TOK_LINESTART  TOK_SEMICOLON {
			$$ = create_schematic_statement(LINESTART, NULL,NULL,0,0,0,0);
		}
                |
                TOK_ARC TOK_LEFTPAREN TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON {
			$$ = create_schematic_statement(ARC, NULL,NULL,$3,$5,$7,0);
		}
                |
                TOK_COORD TOK_LEFTPAREN  TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON {
			$$ = create_schematic_statement(COORD, NULL,NULL,$3,$5,0,0);
		}
                |
                TOK_POLYGONEND TOK_SEMICOLON {
			$$ = create_schematic_statement(POLYGONEND, NULL,NULL,0,0,0,0);
		}
                |
                TOK_LINEEND  TOK_SEMICOLON {
			$$ = create_schematic_statement(LINEEND, NULL,NULL,0,0,0,0);
		}
                |
		TOK_SETGATEPOS TOK_LEFTPAREN TOK_IDENTIFIER TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
                {
			$$ = create_schematic_statement(SETPOS, $3, NULL, $5, $7,0,0);
                }
                |
		TOK_SETGATEROT TOK_LEFTPAREN TOK_IDENTIFIER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
                {
			$$ = create_schematic_statement(SETROT, $3, NULL, $5, 0,0,0);
                }
                |
		TOK_STARTROUTE TOK_LEFTPAREN TOK_IDENTIFIER TOK_POINT TOK_IDENTIFIER TOK_RIGHTPAREN TOK_SEMICOLON
		{
			$$ = create_schematic_statement(STARTROUTE, $3, $5,0,0,0,0);
		}
                |
		TOK_ENDROUTE TOK_LEFTPAREN TOK_IDENTIFIER TOK_POINT TOK_IDENTIFIER TOK_RIGHTPAREN TOK_SEMICOLON
		{
			$$ = create_schematic_statement(ENDROUTE, $3, $5,0,0,0,0);
		}
                |
		TOK_JUNCTION TOK_LEFTPAREN TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
		{
		  $$ = create_schematic_statement(JUNCTION, NULL,NULL,$3, $5,0,0);
		}
                |
		TOK_FIXED_JUNCTION TOK_LEFTPAREN TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
		{
		  $$ = create_schematic_statement(FIXEDJUNCTION, NULL,NULL,$3, $5,0,0);
		}
                |
		TOK_POPJUNCTION TOK_SEMICOLON
		{
		  $$ = create_schematic_statement(POPJUNCTION, NULL,NULL,0, 0,0,0);
		}
                |
		TOK_SYMBOLTEXT  TOK_LEFTPAREN TOK_STRING TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
		{
                        $$ = create_schematic_statement(SYMBOLTEXT, $3, NULL,$5, $7,0,0);
		}
                |
		TOK_TEXT  TOK_LEFTPAREN TOK_STRING TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
		{
		  $$ = create_schematic_statement(SCHEMATICTEXT, $3, NULL,$5, $7,$9,0);
		}
                |
		TOK_STARTTEXT  TOK_LEFTPAREN TOK_STRING TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_COMMA TOK_INTEGER TOK_RIGHTPAREN TOK_SEMICOLON
		{
		  $$ = create_schematic_statement(SCHEMATICSTARTTEXT, $3, NULL,$5, $7,$9,0);
		}
                ;
	

schematic_stmts: schematic_stmts schematic_stmt
              {
                      $$ = $1;
		      struct schematic_statement *stmt = $1;
		      while(stmt->next){
			      stmt = stmt->next;
		      }
                      stmt->next = $2;
              }
              |
	      schematic_stmt
              {
                      $$ = $1;
              }
              ;



        


parameter_insts:
        { $$ = NULL; }
        |
                TOK_HASH TOK_LEFTPAREN parameter_inst TOK_RIGHTPAREN
	{
		$$ = $3;
	}
                
        ;

parameter_inst:
                parameter_inst TOK_COMMA parameter_assign {
        struct module_parameter *tmp = $1;
         tmp->next = $3;
        $$ = tmp;
 }
        |
                parameter_assign { $$ = $1;}
        ;

parameter_assign:
                         TOK_POINT TOK_IDENTIFIER TOK_LEFTPAREN constexpression TOK_RIGHTPAREN { $$ = newmodparam($2, $4);}
        ;


signal_insts:
        signal_insts TOK_COMMA TOK_POINT TOK_IDENTIFIER TOK_LEFTPAREN constexpression TOK_RIGHTPAREN
	{
		struct portconnection *tmp = $1;
		$$ = $1;
		if($$->next){
			tmp=$$->next;
			while(tmp->next){
				tmp = tmp->next;
			}
			tmp->next = create_new_portconn($4, $6);
		}else{
			$$->next = create_new_portconn($4, $6);
		}
	}
     
        |
                TOK_POINT TOK_IDENTIFIER TOK_LEFTPAREN constexpression TOK_RIGHTPAREN
        { 
		printf("New conn   : %s\n", $2);
		$$ = create_new_portconn($2, $4);
	}
        ;


modulestmt:     TOK_MODULE TOK_IDENTIFIER parameterdef inputoutputs TOK_SEMICOLON
                {
			struct module *tmp = (struct module *) malloc(sizeof(struct module));
			if(!tmp) abort();
			tmp->wires = NULL;
			tmp->insts = NULL;
			tmp->modulename = $2;
			tmp->params = $3;
			tmp->ports = $4;
			$$ = tmp;
		}
                ;

parameterdef:
         { $$ = NULL; }
        |
	TOK_HASH TOK_LEFTPAREN TOK_PARAMETER parameters TOK_RIGHTPAREN { $$ = $4;}
                ;

parameters:
      parameters TOK_COMMA parameter {
	      $$ = $1;
	      $$->next = $3;
      }
      |
      parameter { $$ = $1;}
      ;


parameter:
      TOK_IDENTIFIER  TOK_EQUAL TOK_INTEGER
      {
	      $$ = newmodparam($1, constexpr_integer($3));
      }
      ;



inputoutputs:
                { $$ = NULL; }
        |
        TOK_LEFTPAREN inputoutputstatements TOK_RIGHTPAREN { $$ = $2; module_port_print($2); }
       ;


inputoutputstatements:
       directionstatement signaltype bitwidthstatement signallist TOK_COMMA inputoutputstatements
                {
                        struct module_port *tmp = $4;
                        while(tmp){
                                tmp->direction = $1;
                                if($3){
                                        tmp->highrange = $3->highrange;
                                        tmp->lowrange = $3->lowrange;
                                }else{
                                        tmp->highrange = NULL;
                                        tmp->lowrange = NULL;
                                }
				if(!tmp->next){
					tmp->next = $6;
					tmp = NULL;
				}else{
					tmp=tmp->next;
				}
                        }
                        $$ = $4;
                }
       |
       directionstatement signaltype bitwidthstatement signallist
                {
                        struct module_port *tmp = $4;
                        while(tmp){
                                tmp->direction = $1;
                                if($3){
                                        tmp->highrange = $3->highrange;
                                        tmp->lowrange = $3->lowrange;
                                }else{
                                        tmp->highrange = NULL;
                                        tmp->lowrange = NULL;
                                }
                                tmp=tmp->next;
                        }
                        $$ = $4;
                }
       ;




directionstatement:
TOK_INPUT { $$ = INPUT; }
       |
       TOK_OUTPUT { $$ = OUTPUT; }
       ;
// 
signaltype:

       TOK_WIRE
       |
       TOK_REG
       ;

bitwidthstatement:
                { $$ = NULL; }
       |
       TOK_LEFTBRACKET constexpression TOK_COLON constexpression TOK_RIGHTBRACKET { $$ = create_new_port(NULL, UNKNOWN, $2, $4); }
       ;

constexpression:
       TOK_TILDE constexpression { $$ = constexpr_treenode(IS_TILDE, $2, NULL); }
       |
       TOK_IDENTIFIER { $$ = constexpr_identifier($1); }
       |
       TOK_INTEGER { $$ = constexpr_integer($1); }
       |
       TOK_IDENTIFIER TOK_MINUS TOK_INTEGER { $$ = constexpr_treenode(IS_MINUS, constexpr_identifier($1), constexpr_integer($3)); }
       ;


signallist:
       signallist TOK_COMMA TOK_IDENTIFIER  {
	 struct module_port *newport = create_new_port($3, UNKNOWN, NULL, NULL);
	 $$ = $1;
         struct module_port *tmp = $1;
         while(tmp->next){
                 tmp = tmp->next;
         }
	 tmp->next = newport;
       }
       |
       TOK_IDENTIFIER    { $$ = create_new_port($1, UNKNOWN, NULL, NULL); }
       ;
