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
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <string>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include <cairo-svg.h>
#include "verilog-subset.h"
#include "verilog-subset.tab.h"
#include "verilog-subset.yy.h"

#include "gate.hh"
#include "design.hh"
#include "designgui.hh"



extern int yydebug;

int main (int argc, char *argv[])
{
        const char *savefile = NULL;
        const char *verilogfile = NULL;
        bool savepdf = false;
        
        schematic_design *thedesign;
        gtk_init (&argc, &argv);

        int c;
        while(( c = getopt(argc, argv, "df:s:")) != -1){
                switch(c){
                case 'd':
                        fprintf(stderr,"Enabled parser debugging\n");
                        yydebug = 1;
                        break;
                case 's':
                        savefile = optarg;
                        break;
                default:
                        printf("Unknown option %c\n",'c');
                        exit(1);
                }
        }

	if(optind == argc - 1){
		verilogfile = argv[optind];
	}else if(optind == argc){
	  verilogfile = NULL;
	}else{
		fprintf(stderr, "Usage: %s [options] design.v\n", argv[0]);
		exit(1);
	}

        thedesign = new schematic_design(verilogfile);

        if(savefile){
                if(!savepdf){
                        thedesign->save_svg(savefile);
                        exit(0);
                }
        }


        schematic_gui *gui = new schematic_gui(thedesign, verilogfile);

        gtk_main ();

        delete gui;
        delete thedesign;
        
        return 0;
}

