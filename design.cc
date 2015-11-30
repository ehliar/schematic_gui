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
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include <cairo-svg.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>


#include "gate.hh"
#include "verilog-subset.h"
#include "verilog-subset.tab.h"
#include "verilog-subset.yy.h"

#include "design.hh"





extern int start_token;
extern int yyparse(void);


void schematic_design::synch_shaperefs()
{
	shaperef_to_gates.clear();
        for(const auto &  g : gates){
                shaperef_to_gates[g->shapeRef] = g;
	}
}


void schematic_design::trace_connref(Avoid::ConnRef *connref, bool startsearch, FILE *fp, int junctionlevel)
{
             
	if(startsearch){
		connected_connrefs.clear();
		connected_junctions.clear();
	}
        if(!connref){
                return;
        }

        // Don't visit a visited connection again.
        if(connected_connrefs.count(connref) > 0){
                return;
        }

	connected_connrefs.emplace(connref);
	std::unordered_set<Avoid::ConnRef *> foundsofar;

	std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();

	if(fp){
		const char *tmp;

		if(junctionlevel > 0){
			tmp = "endroute";
		}else{
			tmp = "startroute";
		}
		if(endpoints.first.shape()){
			gate * g = shaperef_to_gates[endpoints.first.shape()];
			if(g->istext){
				fprintf(fp, "        $schematic_%s(\"%s.%s\");\n",tmp,
					g->getinstancename().c_str(),
					g->get_connector_name(endpoints.first.pinClassId()).c_str());

			}else{
				fprintf(fp, "        $schematic_%s(%s.%s);\n", tmp,
					g->getinstancename().c_str(),
					g->get_connector_name(endpoints.first.pinClassId()).c_str());

			}
			if(!junctionlevel){
				junctionlevel++;
			}
		}

		if(junctionlevel > 0){
			tmp = "endroute";
		}else{
			tmp = "startroute";
		}
		if(endpoints.second.shape()){
			gate * g = shaperef_to_gates[endpoints.second.shape()];
			if(g->istext){
				fprintf(fp, "        $schematic_%s(\"%s.%s\");\n",tmp,
					g->getinstancename().c_str(),
					g->get_connector_name(endpoints.second.pinClassId()).c_str());
			}else{
				fprintf(fp, "        $schematic_%s(%s.%s);\n", tmp,
					g->getinstancename().c_str(),
					g->get_connector_name(endpoints.second.pinClassId()).c_str());
			}
			if(!junctionlevel){
				junctionlevel++;
			}
		}

	}
	if(endpoints.first.junction()){
		// FIXME: Fixed junction?
		Avoid::JunctionRef * j = endpoints.first.junction();
		if(connected_junctions.count(j) == 0){
			connected_junctions.emplace(j);
			if(fp){
				junctionlevel++;
				if(j->positionFixed()){
					fprintf(fp, "        $schematic_fixed_junction(%d, %d);\n",
						(int)j->recommendedPosition().x,
						(int)j->recommendedPosition().y);
				}else{
					fprintf(fp, "        $schematic_junction(%d, %d);\n",
						(int)j->recommendedPosition().x,
						(int)j->recommendedPosition().y);
				}
				
			}

			Avoid::ConnRefList l = endpoints.first.junction()->attachedConnectors();
			for(const auto & c : l){
				trace_connref(c, false, fp, junctionlevel);
			}
			if(fp){
				fprintf(fp, "        $schematic_popjunction;\n");
			}
		}
	}
	if(endpoints.second.junction()){
		Avoid::JunctionRef * j = endpoints.second.junction();
		if(connected_junctions.count(j) == 0){
			connected_junctions.emplace(j);
			if(fp){
				junctionlevel++;
				if(j->positionFixed()){
					fprintf(fp, "        $schematic_fixed_junction(%d, %d);\n",
						(int)j->recommendedPosition().x,
						(int)j->recommendedPosition().y);
				}else{
					fprintf(fp, "        $schematic_junction(%d, %d);\n",
						(int)j->recommendedPosition().x,
						(int)j->recommendedPosition().y);
				}
				
			}
			Avoid::ConnRefList l = endpoints.second.junction()->attachedConnectors();
			for(const auto & c : l){
				trace_connref(c, false, fp, junctionlevel);
			}
			if(fp){
				fprintf(fp, "        $schematic_popjunction;\n");
			}
		}
	}
}


bool schematic_design::is_same_wire(Avoid::ConnRef *connref1, Avoid::ConnRef *connref2)
{
	trace_connref(connref1, true);
	if(connected_connrefs.count(connref2) > 0){
		return true;
	}else{
		return false;
	}
}

void schematic_design::set_connref_name_all(Avoid::ConnRef *connref, std::string thename)
{
	assert(thename.size() > 0);
        trace_connref(connref, true);
        for(const auto &c : connected_connrefs){
		printf("Setting on %p", c);
                connref_names[c] = thename;
        }
        
}


void schematic_design::set_connref_name(Avoid::ConnRef *connref, std::string thename)
{
	// Could be an alias for set_connref_name_all
	assert(thename.size() > 0);
	connref_names[connref] = thename;
}


std::string schematic_design::get_connref_name(Avoid::ConnRef *connref)
{
	assert(connref_names.count(connref) > 0);
	return connref_names[connref];
}

double schematic_design::check_distance(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

double schematic_design::check_distance(double x1, double y1, Avoid::JunctionRef *j)
{
	return check_distance(x1,y1, j->position().x, j->position().y);
}

extern FILE *yyin;
gate * schematic_design::create_new_gate(const char *filename, const char *instname)
{
        int i=0;
        char instancename[100];
        gate *thegate;
	printf("Creating new gate %s\n", filename);

        // This is such an ugly hack... gate.cc should really be split
        // into gate.cc and text.cc
        if(!strcmp(filename,".text")){
                thegate = new gate(router, ".text");

		if(!instname){
			do{
				sprintf(instancename, "text_inst_%02d", i++);
			}while(gate_names.count(instancename) > 0);
			instname = &instancename[0];
		}
        }else{
                char buf[500];
                module_parse_info = NULL;
                snprintf(buf, 490, "%s.v",filename);
                FILE *fp = fopen(buf,"r");
                if(!fp){
			snprintf(buf, 490, PREFIX "/share/schematic_gui/components/%s.v", filename);
			fprintf(stderr, "Could not open file in current directory, retrying at %s\n", buf);
			fp = fopen(buf,"r");
			if(!fp){
				printf("Could not open file %s\n", buf);
				return NULL;
			}
                }
                yyrestart(fp);
		start_token = START_WITH_SUBMODULE_PARSER;
                yyparse();
                fclose(fp);
                if(module_parse_info){
			// For debugging
                        // print_module(module_parse_info);
                        thegate = new gate(router, module_parse_info);
                }else{
                        return NULL;
                }
        }
        if(!instname){
                do{
                        sprintf(instancename, "inst_%02d", i++);
                }while(gate_names.count(instancename) > 0);
                thegate->setinstancename(instancename);
        }else{
                thegate->setinstancename(instname);
        }

        
        if(gate_names.count(thegate->getinstancename())){
                fprintf(stderr,"Error: A gate with name %s already exists!\n",
			thegate->getinstancename().c_str());
                abort();
        }
        
        gate_names[thegate->getinstancename()] = thegate;
        
	gates.emplace(thegate);
	return thegate;

}

void schematic_design::draw_junction(cairo_t *cr, Avoid::JunctionRef *j, bool is_ui)
{
	if(j->positionFixed()){
		double x;
		double y;

                // Don't draw this when exporting the schematic
                if(!is_ui){
                        return;
                }

		cairo_save(cr);
		if(selected_fixed_junctions.find(j) != selected_fixed_junctions.end()){
                        cairo_set_source_rgb (cr, 0.5, 0.2, 0.2);
		}			
		x = j->position().x;
		y = j->position().y;		
		cairo_move_to(cr, x-10, y-10);
		cairo_line_to(cr, x+10, y+10);
		cairo_stroke(cr);
		cairo_move_to(cr, x+10, y-10);
		cairo_line_to(cr, x-10, y+10);
		cairo_stroke(cr);
		cairo_restore(cr);
		
	}else{
		cairo_arc(cr, j->recommendedPosition().x, j->recommendedPosition().y, 4,0,2*M_PI);
		cairo_fill(cr);
		
	}
}

void schematic_design::draw_checkpoint(cairo_t *cr, Avoid::Checkpoint c)
{
//        cairo_new_path(cr);
        cairo_move_to(cr, c.point.x-10, c.point.y-10);
        cairo_line_to(cr, c.point.x+10, c.point.y+10);
        cairo_stroke(cr);
        cairo_move_to(cr, c.point.x+10, c.point.y-10);
        cairo_line_to(cr, c.point.x-10, c.point.y+10);
        cairo_stroke(cr);
}

void schematic_design::draw_connref(cairo_t *cr, Avoid::ConnRef *connref, bool is_ui)
{
	Avoid::Polygon p = connref->displayRoute();
	std::vector<Avoid::Point> ps = p.ps;
	if(ps.size() == 0){
		return;
	}
	cairo_save(cr);
	if(connected_connrefs.count(connref) > 0){
		if(is_ui){
			cairo_set_source_rgb (cr, 1, 0.5, 0.5);
		}
	}
	if(connref == closestline){
		if(is_ui){
			cairo_set_source_rgb (cr, 1, 0.0, 0.0);
		}
	}
	cairo_move_to(cr, ps[0].x, ps[0].y);
	for(const auto & thepoint : p.ps){
		cairo_line_to(cr, thepoint.x, thepoint.y);
	}
	cairo_stroke(cr);

	// Removed this since checkpoints don't actually seem to
	// work very well in libavoid (unfortunately).  Junctions
	// with fixed locations are used instead.
	////            std::vector<Avoid::Checkpoint> cp = connref->routingCheckpoints();
	////            for(std::vector<Avoid::Checkpoint>::iterator it = cp.begin(); it < cp.end(); it++){
	////                    draw_checkpoint(cr,*it);
	////            }

	std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
	if(endpoints.first.junction()){
		draw_junction(cr,endpoints.first.junction(), is_ui);
	}
	if(endpoints.second.junction()){
		draw_junction(cr,endpoints.second.junction(), is_ui);
	}
	cairo_restore(cr);
	
}

// FIXME: Make sure that top level inputs/outputs are visible in internal netlist!

void schematic_design::save_verilog(std::string filename)
{
	
        FILE *fp = fopen(filename.c_str(),"w");
	fprintf(fp, "/* Save file generated by schematic_gui v" SCHEMATIC_VERSION " */\n");
        if(!fp){
                // FIXME: Pop up big ugly error message here!
                perror("Could not open save file");
                return;
        }

        fprintf(fp,"module test;\n"); // FIXME: Name!
	synch_shaperefs();
        std::unordered_set<std::string> wirenames;

	// Find all wirenames in the schematic
        for(const auto & c : router->connRefs){
                wirenames.emplace(get_connref_name(c));
        }
	// Output all wirenames in the schematic
	// FIXME: Handle width of wires somehow
        for(const auto & s : wirenames){
		if(s.length() == 0){
			fprintf(stderr,"BUG: no wirename (not even an autogenerated one, ignoring this connref).\n");
		}else{
			fprintf(fp, "    wire %s;\n", s.c_str());
		}
        }

        fprintf(fp, "\n");

        unsigned int j;
	unsigned int unknown_wireid = 0;
        for(const auto &g : gates){
                bool firstport = true;
                if(g->istext) {
                        continue;
                }

                fprintf(fp,"    %s %s(", g->getname().c_str(), g->getinstancename().c_str());

                // FIXME: Time to fix +1/-1 in regards to pin numbering, this is really bad.
                for(j=1; j <= g->maxpins(); j++){
                        
			Avoid::ConnEnd endpoint = g->getConnEnd(j-1);
			
			
			
			Avoid::ConnRef *connref = g->get_connref_by_pin(j);
			
			std::string wname;
			if(connref){
				wname = get_connref_name(connref);
				if(wname.empty()){
					char buf[100];
					sprintf(buf,"UNKNOWN_WIRE_SHOULD_NOT_HAPPEN_%d", unknown_wireid++);
					wname = buf;
					abort();
					// set_connref_name(connref, wname);
				}
				if(!firstport){
					fprintf(fp,",\n        ");
				}
				firstport = false;
				fprintf(fp, ".%s(", g->get_connector_name(j).c_str());
				if(g->isinverted(j-1)){
					fprintf(fp,"~");
				}
				fprintf(fp, "%s)", wname.c_str());
			}else{
				wname = "";
			}
                }
                fprintf(fp,");\n\n");
        }

	fprintf(fp,"    initial begin\n");
        for(const auto & g : gates){
		const char *delimiter = "";
                Avoid::Point p = g->getpos();
		if(g->istext) {
			fprintf(fp, "        $schematic_text(\"%s\", \"%s\");\n",
				g->getinstancename().c_str(),
				g->gettext().c_str());
			delimiter = "\"";
		}

		

		fprintf(fp,"        $schematic_setgatepos(%s%s%s, %d,%d);\n", delimiter, g->getinstancename().c_str(),delimiter, 
			(int)p.x, (int) p.y);
		int rot = g->getrotation();
		if(rot){
			fprintf(fp, "        $schematic_setgaterot(%s%s%s, %d);\n", delimiter, g->getinstancename().c_str(), delimiter, rot);
		}
	}

	connected_connrefs.clear();
	connected_junctions.clear();
        for(const auto & connref : router->connRefs){
		std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
		if(endpoints.first.shape() || endpoints.second.shape()){
			if(connected_connrefs.count(connref) == 0){
				trace_connref(connref, false, fp,0);
			}
		}
			
	}
	fprintf(fp, "    end\n");
	
                                                      
        fprintf(fp,"endmodule\n");
        fclose(fp);
}

void schematic_design::draw_with_cairo(cairo_t *cr, bool is_ui)
{
        /* Set color for background */
	cairo_scale(cr, cairo_scale_factor, cairo_scale_factor);
	cairo_translate(cr, cairo_translation_x, cairo_translation_y);
	
	
        cairo_set_line_cap  (cr, CAIRO_LINE_CAP_SQUARE);
        /* fill in the background color*/

        // Note: We don't fill the background with white if we export
        // to SVG/etc since we do not yet have a way to export only
        // the components in the schematic. 
        if(is_ui){
                cairo_set_source_rgb(cr, 1, 1, 1);
                cairo_rectangle(cr, 1,1,7998,7998); // Draw outline of schematic sheet
                cairo_fill(cr);

                cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
                cairo_rectangle(cr, 1,1,7998,7998); // Draw outline of schematic sheet
                cairo_stroke(cr);
                
        }
        cairo_set_source_rgb(cr, 0, 0, 0);
     


        // Draw connectors
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 3);
        for(const auto &c : router->connRefs){
                draw_connref(cr,c, is_ui);
        }

        for(const auto & g : gates){
		cairo_save(cr);
		if(is_ui){
                        if((closest_gate == g) && closest_obstacle_is_gate){
				cairo_set_source_rgb (cr, 0.5, 0.2, 0.2);
                        }
			if(selected_gates.find(g) != selected_gates.end()){
				cairo_set_source_rgb (cr, 0.5, 0.0, 0.0);
			}
                        
		}
                        
                g->draw(cr, is_ui);
		cairo_restore(cr);
        }
        
        if(!is_ui){
                return;
        }
        cairo_set_source_rgb (cr, 1, 0.2, 0.2);
        if(closest_fixed_junction && !closest_obstacle_is_gate){
//                cairo_move_to(cr,closest_fixed_junction->position().x, closest_fixed_junction->position().y);
                cairo_arc(cr, closest_fixed_junction->position().x, closest_fixed_junction->position().y, 20,0,2*M_PI);
                cairo_stroke(cr);
        }

        if(closestline){
//                cairo_move_to(cr,closestlinex, closestliney);
                cairo_arc(cr, closestlinex, closestliney, 10,0,2*M_PI);
                cairo_stroke(cr);
        }

        if(current_gate_pin){
                cairo_arc(cr, closestconnectorx, closestconnectory, 5,0,2*M_PI);
                cairo_stroke(cr);
        }




        return;
}


void schematic_design::save_svg(std::string filename)
{
        cairo_surface_t *surface;
        cairo_t *cr;
        surface = cairo_svg_surface_create(filename.c_str(),8000,8000);
        cr = cairo_create(surface);
        draw_with_cairo(cr,false);
        cairo_surface_destroy(surface);
        cairo_destroy(cr);
}


// FIXME: Refactor to use  Avoid::Point?
double schematic_design::find_distance(double x1, double y1, double x2, double y2, double x, double y, double *closestx, double *closesty)
{
	if((x1 != x2) && (y1 != y2)){
		*closestx = x;
		*closesty = y;
		return 9999999999; // FIXME: Refactor
	}

	double tmp;
	if(x1 == x2){
		if(y2 < y1){
			tmp = y2;
			y2 = y1;
			y1 = tmp;
		}
		if(y <= y1){
			*closestx = x1;
			*closesty = y1;
			return check_distance(x1,y1,x,y);
		}
		if(y >= y2){
			*closestx = x2;
			*closesty = y2;
			return check_distance(x2,y2,x,y);
		}
		*closestx = x1;
		*closesty = y;
		return fabs(x1-x);
	}else{ // y1 == y2
		if(x2 < x1){
			tmp = x2;
			x2 = x1;
			x1 = tmp;
		}
		if(x <= x1){
			*closestx = x1;
			*closesty = y1;
			return check_distance(x1,y1,x,y);
		}
		if(x >= x2){
			*closestx = x2;
			*closesty = y2;
			return check_distance(x2,y2,x,y);
		}
		*closestx = x;
		*closesty = y1;
		return fabs(y1-y);
		
	}
	
	
}

double schematic_design::find_closest_line_in_route(double x, double y, double *closestx, double *closesty, Avoid::ConnRef *r)
{
	int segment = 0;
	Avoid::Polygon p = r->displayRoute();
	std::vector<Avoid::Point> ps = p.ps;

	double prevx, prevy;
	double olddistance = 999999999; // FIXME: Safe in practice, if not in theory.


	for(const auto & thepoint : p.ps){
		if(!segment){
			prevx = thepoint.x;
			prevy = thepoint.y;
			segment++;
			continue;
		}else{
			double tmpx, tmpy;
			double newdist = find_distance(prevx, prevy, thepoint.x, thepoint.y,x,y,&tmpx,&tmpy);
			if(newdist <olddistance){
				olddistance = newdist;
				*closestx = tmpx;
				*closesty = tmpy;
			}
			prevx = thepoint.x;
			prevy = thepoint.y;
		}
	}
	return olddistance;
}


void schematic_design::update_closest_fixed_junction(double x, double y, double *distance, Avoid::JunctionRef **bestsofar, Avoid::JunctionRef *junction)
{
	double newdistance;
	if(!junction){
		return;
	}
	if(!junction->positionFixed()){
		return;
	}

	newdistance = check_distance(x,y,junction);
	if(*bestsofar){
		if(newdistance >= *distance){
			return;
		}
	}
	*distance = newdistance;
	*bestsofar = junction;
	
}
Avoid::JunctionRef *schematic_design::find_closest_fixed_junction(double x, double y)
{
	Avoid::JunctionRef *bestsofar = NULL;
	double distance;
	for(const auto & c : router->connRefs){
		std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = c->endpointConnEnds();
		update_closest_fixed_junction(x,y,&distance,&bestsofar,endpoints.first.junction());
		update_closest_fixed_junction(x,y,&distance,&bestsofar,endpoints.second.junction());
	}
	return bestsofar;
}


Avoid::ConnRef * schematic_design::find_closest_line_segment(double x, double y, double *xref, double *yref)
{
	double distance=99999999;
	Avoid::ConnRef *retval = NULL;
	*xref = x;
	*yref = y;
	
        for(const auto &c : router->connRefs){
		double tmpx,  tmpy;
		double newdistance;
		if(c == temporary_route){
			continue;
		}
		
		newdistance = find_closest_line_in_route(x,y,&tmpx, &tmpy, c);
		if(newdistance < distance){
			*xref = tmpx;
			*yref = tmpy;
			retval = c;
			distance = newdistance;
		}
	}
	return retval;
}

                

void schematic_design::move_current_obstacle(double x, double y)
{
	
	x = ((int)x / 10)*10;
	y = ((int)y / 10)*10;

	double diffx = x - previous_x;
	double diffy = y - previous_y;
	previous_x = x;
	previous_y = y;

	if((diffx == 0) && (diffy == 0)){
		return;
	}
	
        if(!obstacle_is_selected){
                return;
        }

	if((selected_gates.size() > 0) || (selected_fixed_junctions.size() > 0)){
		for(const auto & g : selected_gates){
			g->movedelta(diffx,diffy);
		}
		for(const auto &j : selected_fixed_junctions){
			router->moveJunction(j,diffx,diffy);
		}
	}else if(current_gate_is_closest){
		current_gate->move(x,y);
        }else{
		if(!closest_fixed_junction){
			return;
		}
		router->moveJunction(closest_fixed_junction,Avoid::Point(((int)x/10)*10,((int)y/10)*10));
        }

        router->processTransaction();
}

gate * schematic_design::find_closest_gate(double x, double y, double *distance)
{
	double closest_distance;
        bool first = true;
        gate *bestgate = NULL;
        
        for(const auto & g : gates){
                double newdist = g->getdistance(x,y);
                if(first || (newdist < closest_distance)){
                        first = false;
                        closest_distance = newdist;
                        bestgate = g;
                }
	}

        *distance = closest_distance;
        
	return bestgate;
}

void schematic_design::select_closest_pin(double x, double y)
{
	int currentpin;
	int pin;
	double closest_distance;
        bool first = true;;
	double newdist;
        gate * bestgate = NULL;
        for(const auto & g : gates){
		newdist = g->getclosestpin(x,y, &pin);
		if(first || (newdist < closest_distance)){
			currentpin = pin;
                        bestgate = g;
			closest_distance = newdist;
                        first = false;
		}
	}
        if(!bestgate){
                current_gate_pin = NULL;
                return;
        }
	bestgate->get_connector_coords(currentpin, &closestconnectorx, &closestconnectory);
	currentConnEnd = bestgate->getConnEnd(currentpin);

	current_gate_pin = bestgate;
	current_pin = currentpin;
}

void schematic_design::remove_unused_junctions()
{
        for(const auto & c : router->connRefs){
                std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = c->endpointConnEnds();
                Avoid::JunctionRef *j;
                j = endpoints.first.junction();
                if(j){
                        if(!j->positionFixed()){
                                if(j->removeJunctionAndMergeConnectors()){
					router->processTransaction();
                                        // Start over again.
                                        remove_unused_junctions();
                                        return;
                                }
                        }
                }
                j = endpoints.second.junction();
                if(j){
                        if(!j->positionFixed()){
                                if(j->removeJunctionAndMergeConnectors()){
					router->processTransaction();
                                        // Start over again.
                                        remove_unused_junctions();
                                        return;
                                }
                        }
                }
	}
        router->processTransaction();
}

void schematic_design::add_checkpoint_to_closest_line(double x, double y)
{
	if(!closestline){
		fprintf(stderr,"WARNING: No closestline set, ignoring checkpoint addition\n");
		return;
	}

	std::string wirename = get_connref_name(closestline);
 	std::pair<Avoid::JunctionRef *, Avoid::ConnRef *> tmp = closestline->splitAtSegment(1);
 	tmp.first->setPositionFixed(true);
 	router->moveJunction(tmp.first, Avoid::Point(x, y));
        router->setRoutingParameter(Avoid::segmentPenalty, 2);
        router->processTransaction();

	set_connref_name(tmp.second,wirename);

	obstacle_is_selected = false;
	current_gate_is_closest = false;
        
}

void schematic_design::clean_wirenames()
{
	std::unordered_set<Avoid::ConnRef *> valid_connrefs;
	for(const auto & c : router->connRefs){
		printf("Emplaced %p\n", c);
		valid_connrefs.emplace(c);
	}
	std::unordered_map<Avoid::ConnRef *, std::string> new_names;
	for(const auto & c : connref_names){
		printf("Looking for %p\n", c.first);
		if(valid_connrefs.count(c.first) > 0){
			assert(c.second.size() > 0);
			new_names[c.first] = c.second;
			printf("Added wirename %s\n", c.second.c_str());
		}
	}
	connref_names = new_names;
}

// FIXME: Name of function is not totally correct (removes junctions
// with two connections (e.g. unused junctions)
void schematic_design::remove_connref_and_fixed_junctions(Avoid::ConnRef *connref)
{
        std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
        Avoid::JunctionRef *j;
        j = endpoints.first.junction();
        if(j){
                Avoid::ConnRef *r = j->removeJunctionAndMergeConnectors();
                if(r){
			router->processTransaction();
                        // Ok, try to remove the now dangling connref returned in r
                        remove_connref_and_fixed_junctions(r);
                        return;
                }
        }
        
        j = endpoints.second.junction();
        if(j){
                Avoid::ConnRef *r = j->removeJunctionAndMergeConnectors();
                if(r){
			router->processTransaction();
                        // Ok, try to remove the now dangling connref returned in r
                        remove_connref_and_fixed_junctions(r);
                        return;
                }
        }
	// Note: clean_wirenames() must be run after this function

        router->deleteConnector(connref);
	router->processTransaction();
}

void schematic_design::remove_closest_object(double x, double y)
{
        if(find_closest_obstacle(x,y)){
                bool finished = false;
                if(!closest_gate){
                        return;
                }
                Avoid::ShapeRef *s = closest_gate->shapeRef;

                while(!finished){
                        Avoid::ConnRefList l = s->attachedConnectors();
                        if(l.size() > 0){
                                remove_connref_and_fixed_junctions(l.front());
				clean_wirenames();
                                closest_fixed_junction = NULL;
                        }else{
                                finished = true;
                        }
                }
                router->deleteShape(closest_gate->shapeRef);
		gate_names.erase(closest_gate->getinstancename());
                gates.erase(closest_gate);
        }else{
                if(closest_fixed_junction){
			// FIXME: Note, memory leak. This function
			// does not (according to adaptagram
			// documentation) delete and free the Junction
			// itself
			Avoid::ConnRefList thelist = closest_fixed_junction->attachedConnectors();
			std::vector<Avoid::ConnRef *> l{std::begin(thelist), std::end(thelist) };
			if(l.size() != 2){
				fprintf(stderr,"Error, unexpected connreflist size\n");
				abort();
			}
			Avoid::ConnRef *newconnref = closest_fixed_junction->removeJunctionAndMergeConnectors();
			router->processTransaction();
			if(l[0] == newconnref){
				connref_names.erase(l[1]);
			}else if(l[1] == newconnref){
				connref_names.erase(l[0]);
			}else{
				// Sanity check, should not happen
				fprintf(stderr,"Error, could not erase old connref name\n");
				abort();
			}
                }
                closestline = NULL;
                closest_fixed_junction = NULL;
        }
	router->processTransaction();
	obstacle_is_selected = false;
}


// Note: http://ubuntuforums.org/showthread.php?t=1423548
static void enter_callback(GtkWidget *widget, GtkWidget *dialog)
{
	g_signal_emit_by_name(G_OBJECT(dialog), "response", GTK_RESPONSE_ACCEPT);
}

// From http://www.gtkforums.com/viewtopic.php?f=3&t=178427
std::string show_entry_dialog(std::string current_text = "")
{
        
	GtkWidget *dialog;
	GtkWidget *entry;
	GtkWidget *content_area;

	dialog = gtk_dialog_new();
	gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", 0);
	//    gtk_dialog_add_button(GTK_DIALOG(dialog), "CANCEL", 1);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), current_text.c_str());
	gtk_container_add(GTK_CONTAINER(content_area), entry);
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
    
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	gtk_widget_show_all(dialog);
	guint event_handler_id = g_signal_connect(entry, "activate",
						  G_CALLBACK(enter_callback),
						  (gpointer) dialog);


    
	gtk_dialog_run(GTK_DIALOG(dialog));



    
	std::string entry_line = ""; // variable for entered text
	entry_line = gtk_entry_get_text(GTK_ENTRY(entry));

	std::cout << "Data entered: "
		  << entry_line << std::endl;

	g_signal_handler_disconnect(entry, event_handler_id);
	gtk_widget_destroy(dialog);
	return entry_line;
}

void schematic_design::selection_add()
{
	if(current_gate_is_closest){
		if(closest_gate){
			selected_gates.emplace(closest_gate);
		}
	}else{
		if(closest_fixed_junction){
			selected_fixed_junctions.emplace(closest_fixed_junction);
		}
	}
}

void schematic_design::scale_and_translate(double *x, double *y)
{
	*x = *x / cairo_scale_factor;
	*y = *y / cairo_scale_factor;
	*x = *x - cairo_translation_x;
	*y = *y - cairo_translation_y;
}

void schematic_design::create_new_text_and_attach(double x, double y, const char *edittext)
{
        double pinx, piny;
        double distx, disty;
	const char *tmpstr;

        obstacle_is_selected = true;
        current_gate_is_closest = true;
        current_gate = create_new_gate(".text");

	if(edittext){
		tmpstr = edittext;
	}else{
		tmpstr = show_entry_dialog().c_str();
	}
	fprintf(logfp,"%s\n", tmpstr);
	
	current_gate->settext(tmpstr);
	
        if(!current_gate_pin){
                return;
        }
        current_gate_pin->get_connector_coords(current_pin, &pinx,&piny);
        distx = fabs(pinx-x);
        disty = fabs(piny-y);

	Avoid::ConnRef *tmp;
        if((piny > y) && (distx < disty)){
                tmp = current_gate_pin->connect(current_pin+1, current_gate, 3);
                current_gate->move(pinx,y);
        }else if((piny < y) && (distx < disty)){
                tmp = current_gate_pin->connect(current_pin+1, current_gate, 4);
                current_gate->move(pinx,y);
        }else if((pinx> x) && (disty < distx)){
                tmp = current_gate_pin->connect(current_pin+1, current_gate, 2);
                current_gate->move(x,piny);
        }else{
                tmp = current_gate_pin->connect(current_pin+1, current_gate, 1);
                current_gate->move(x,piny);
        }
        router->processTransaction();
	create_new_wirename(tmp);
}
void schematic_design::add_gate(std::string gatename)
{
        current_gate = create_new_gate(gatename.c_str());
        if(!current_gate){
                obstacle_is_selected = false;
                current_gate_is_closest = false;
        }else{
                obstacle_is_selected = true;
                current_gate_is_closest = true;
        }
}

// FIXME: Move interface related stuff to designgui.cc.
bool schematic_design::handle_key(int keyval)
{
        double x = lastx;
        double y = lasty;
        const char *edittext = NULL;
	bool must_invalidate = true;
	fprintf(logfp, "k %d %d %d\n", keyval, (int)x, (int)y);
	fflush(logfp);
	refresh_highlighted_objects(x,y);
	scale_and_translate(&x,&y);
        if(keyval == 'A'){
                select_all_objects();
	}else if(keyval == 'P'){
		router->outputInstanceToSVG("debug");
        }else if(keyval == 'D'){
                remove_closest_object(lastx, lasty);
                remove_unused_junctions();
		clean_wirenames();
                refresh_highlighted_objects(x,y);
        }else if(keyval == GDK_KEY_Right){
		cairo_translation_x += 16;
	}else if(keyval == GDK_KEY_Left){
		cairo_translation_x -= 16;
        }else if(keyval == GDK_KEY_Up){
		cairo_translation_y -= 16;
	}else if(keyval == GDK_KEY_Down){
		cairo_translation_y += 16;
	}else if(keyval == 'd'){
		if(activeconnend){
			activeconnend = false;
			router->deleteJunction(temporary_junction);
			router->deleteConnector(temporary_route);
			remove_unused_junctions();
			clean_wirenames();
		}else if(closestline){
                        remove_connref_and_fixed_junctions(closestline);
			closestline = NULL;
                        closest_fixed_junction = NULL;
			remove_unused_junctions();
			clean_wirenames();
		}
		router->processTransaction();
		// Save svg from libavoid for debugging!
	}else if(!activeconnend && (keyval == '1')){
                add_gate("add");
	}else if(!activeconnend && (keyval == '2')){
                add_gate("mux2");
	}else if(!activeconnend && (keyval == '3')){
                add_gate("mux3");
	}else if(!activeconnend && (keyval == '4')){
                add_gate("mux4");
	}else if(!activeconnend && (keyval == '5')){
                add_gate("mux5");
	}else if(!activeconnend && (keyval == '6')){
                add_gate("mux6");
	}else if(!activeconnend && (keyval == '7')){
                add_gate("clockedreg");
	}else if(!activeconnend && (keyval == '8')){
                add_gate("mult");
	}else if(!activeconnend && (keyval == '9')){
                add_gate("box");
	}else if(!activeconnend && (keyval == '0')){
                add_gate("bigbox");
        }else if(keyval == 'c'){
                add_checkpoint_to_closest_line(x,y);
	}else if(keyval == 'r'){
                if(closest_gate){
                        closest_gate->rotate(1);
                        router->processTransaction();
			refresh_highlighted_objects(x,y);
                }
        }else if(keyval == 'i'){
                if(current_gate_pin){
                        current_gate_pin->toggleinv(current_pin);
                }
        }else if(keyval == 'e'){
                std::string str;
                if(!closestline){
                        return false;
                }

                std::string netname = get_connref_name(closestline);
                
		if(edittext){
			str = edittext;
		}else{
			str = show_entry_dialog(netname).c_str();
		}

                set_connref_name_all(closestline, str);
                
        }else if(keyval == 'E'){
		const char *str;
		if(!current_gate && !closest_gate){
			return false;
		}
		if(edittext){
			str = edittext;
		}else{
			str = show_entry_dialog().c_str();
		}
		fprintf(logfp, "%s\n", str);
		if(current_gate){
			if(current_gate->istext){
                                current_gate->settext(str);
			}
		}else if(closest_gate){
                        if(closest_gate->istext){
                                closest_gate->settext(str);
                                
                        }
                }
	}else if(keyval == 'U'){
                // Try to work around the fact that junctions in
                // libavoid don't behave the way I want them to.
                // (This doesn't help, but is kept here for debugging
                // purposes until I figure out a good solution.)
		update_all_junctions();
        }else if(keyval == 't'){
                std::string tmpstr;
		if(edittext){
			tmpstr = edittext;
		}else{
			tmpstr = show_entry_dialog();
		}
		fprintf(logfp,"%s\n", tmpstr.c_str());
                printf("AAA: Setting text to %s\n", tmpstr.c_str());
		
                obstacle_is_selected = true;
                current_gate_is_closest = true;
                current_gate = create_new_gate(".text");
		current_gate->settext(tmpstr);
                current_gate->move(x,y);
        }else if(keyval == 'T'){
                create_new_text_and_attach(x,y, edittext);

//         }else if(keyval == 'W'){
//                 printf("SAVING SVG FILE\n");
//                 save_svg("schematic.svg");
//                 must_invalidate = false;
//         }else if(keyval == 'w'){
//                 printf("Saving verilog file\n");
//                 save_verilog("schematic.v");
//                 must_invalidate = false;
	}else if(keyval == 's'){
		selection_add();
	}else if(keyval == 'S'){
		selected_gates.clear();
		selected_fixed_junctions.clear();
        }else{
                must_invalidate = false;
        }

        if(must_invalidate){
                debug_save();
        }

	return must_invalidate;
}

void schematic_design::debug_save()
{
        time_t t;
        struct tm *tmp;
        
        char buf[500];
        t = time(NULL);
        tmp = localtime(&t);
        strftime(buf, 490, "%F_%H%M%S", tmp);
        std::string save_name = getenv("HOME");
        save_name = save_name + "/.schematic_gui";
        mkdir(save_name.c_str(), 0700); // Create save directory if it doesn't exist
        save_name = save_name + "/backup_" + std::string(buf) + ".v";
        save_verilog(save_name);
                
}

void schematic_design::select_all_objects()
{
        for(const auto & g : gates){
                selected_gates.emplace(g);
        }
}

void schematic_design::select_closest_obstacle(double x, double y, bool select)
{
	if(select){
		previous_x = ((int)x/10)*10;
		previous_y = ((int)y/10)*10;
	}
	
        double distance;
        obstacle_is_selected = select;
        closest_gate = find_closest_gate(x, y, &distance);

        if(select){
                current_gate_is_closest = true;
                current_gate = closest_gate;
        }else{
		current_gate = NULL;
        }

        // If there is no closest gate the obstacle is not selected at all.
        if(!closest_gate){
                obstacle_is_selected = false;
        }

//         double cp_distance  = find_closest_checkpoint_in_scene(x,y,&route_with_closest_cp, &closest_cp_in_route);
//         if(cp_distance < distance){
//                 if(select){
//                         current_gate_is_closest = false;
//                 }
//         }
	
 	closest_fixed_junction = find_closest_fixed_junction(x,y);
	if(!closest_fixed_junction){
		return;
	}
	if(check_distance(x,y,closest_fixed_junction) < distance){
		if(select){
			current_gate_is_closest = false;
		}
	}
}

// Return true if a gate is closest, otherwise false
bool schematic_design::find_closest_obstacle(double x, double y)
{
        double distance;
        closest_gate = find_closest_gate(x, y, &distance);
//         if(select){
//                 current_gate_is_closest = true;
//                 current_gate = closest_gate;
//         }else{
// 		current_gate = NULL;
//         }
// 
//         double cp_distance  = find_closest_checkpoint_in_scene(x,y,&route_with_closest_cp, &closest_cp_in_route);

	closest_fixed_junction = find_closest_fixed_junction(x,y);
	if(!closest_fixed_junction){
		return true;
	}
	if(check_distance(x,y,closest_fixed_junction) < distance){
		return false;
	}
	return true;

        
}

void schematic_design::create_new_wirename(Avoid::ConnRef *connref)
{
        // Find all unique wirenames
        std::unordered_set<std::string> wirenames;
        for(const auto & c : router->connRefs){
		if(connref_names.count(c) > 0){
			wirenames.emplace(connref_names[c]);
		}
        }

	unsigned int unknown_wireid = 0;
	std::string newwirename;
	do{
		newwirename = "unnamed_wire" + std::to_string(unknown_wireid++);
	}while(wirenames.count(newwirename) > 0);
	set_connref_name(connref, newwirename);
}

void schematic_design::handle_button(int button, double x, double y)
{
        bool must_save = false;
	fprintf(logfp, "b %d %d %d\n", button, (int)x, (int)y);
	fflush(logfp);
	refresh_highlighted_objects(x,y);
	scale_and_translate(&x,&y);
        if(current_gate){
                if(button != 1){
                        return; 
                }
        }

        if(activeconnend){
                if(button == 1){
                        return;
                }
        }

	// FIXME: Refactor, move to different functions. Add documentation.
	if(button == 2){
		if(!activeconnend){
			selectedConnEnd = currentConnEnd;
                        if(!current_gate_pin){
                                return;
                        }
			selected_gate_pin=current_gate_pin;
			selected_pin = current_pin;
			activeconnend = 1;
			temporary_junction = new Avoid::JunctionRef(router,Avoid::Point(x,y));
			temporary_route = new Avoid::ConnRef(router, Avoid::ConnEnd(temporary_junction), selectedConnEnd);
			router->processTransaction();
                        name_of_first_connref = "";
			previous_closestline = NULL;
		}else{
			previous_closestline = NULL;
                        must_save = true;
			activeconnend = 0;
			router->deleteJunction(temporary_junction);
			router->deleteConnector(temporary_route);
			temporary_route = 0;

			// Make sure we don't try to connect to the same wire
			if((selected_gate_pin != current_gate_pin) ||
			   (selected_pin != current_pin)){
				
				Avoid::ConnRef *newconnref = new Avoid::ConnRef(router, selectedConnEnd, currentConnEnd);
				
				router->processTransaction();
				selected_gate_pin = 0;
				if(name_of_first_connref.empty()){
					create_new_wirename(newconnref);
				}else{
					set_connref_name_all(newconnref, name_of_first_connref);
					printf("new for %p: %s\n", newconnref, name_of_first_connref.c_str());
					name_of_first_connref = "ERROR_THIS_NAME_SHOULD_NOT_BE_USED"; // For sanity checking
				}
			}else{
				printf("Trying to connect to same connend\n");
			}
		}
	}else if(button == 3){
		if(!closestline || is_same_wire(closestline, previous_closestline)){
			if(is_same_wire(closestline, previous_closestline)){
				fprintf(stderr, "Warning: The program does not handle cyclic wire graphs, ignoring this command\n");
			}
			// This might happen if the design is empty except for the first wire
			if(activeconnend){
				activeconnend = false;
				router->deleteJunction(temporary_junction);
				router->deleteConnector(temporary_route);
				remove_unused_junctions();
				clean_wirenames();
			}
				
			activeconnend = false;
			previous_closestline = NULL;
			return;
		}
                std::string oldname; // FIXME: need better name for this variable (name_of_closest_line perhaps)
//                 if(connref_names.count(closestline) == 0){
//                         fprintf(stderr,"Internal error, no name for the selected wire (continuing anyway)\n");
//                         oldname = "EMPTY_NAME_FOR_SELECTED_WIRE";
//                 }else{
//                         oldname = connref_names[closestline];
//                 }
 		oldname = get_connref_name(closestline);
		
		std::pair<Avoid::JunctionRef *, Avoid::ConnRef *> tmp = closestline->splitAtSegment(1);
		set_connref_name(tmp.second, oldname);
		
		router->processTransaction();

		if(activeconnend){
                        must_save = true;
			router->deleteConnector(temporary_route);
			router->deleteJunction(temporary_junction);
			temporary_route = 0;
			router->processTransaction();

			if(is_same_wire(previous_closestline, closestline)){
				fprintf(stderr, "Warning: The program does not handle cyclic wire graphs, ignoring this command\n");
			}else{
				Avoid::ConnRef *newconnref = new Avoid::ConnRef(router, Avoid::ConnEnd(tmp.first), selectedConnEnd);
				router->processTransaction();
				if(name_of_first_connref.size() == 0){
					set_connref_name(newconnref, get_connref_name(closestline));
					printf("new (2) for %p: %s\n", newconnref, name_of_first_connref.c_str());
				}else{
					fprintf(stderr, "FIXME, pop up dialog and give user a choice here: Merging wire with names %s and %s to %s\n",
						oldname.c_str(), name_of_first_connref.c_str(), name_of_first_connref.c_str());
					set_connref_name_all(newconnref, name_of_first_connref);
				}
				name_of_first_connref = "ERROR_THIS_SHOULD_NOT_BE_USED_EITHER";
				activeconnend = false;
				previous_closestline = NULL;
			}

		}else{
			selectedConnEnd = Avoid::ConnEnd(tmp.first);
			temporary_junction = new Avoid::JunctionRef(router,Avoid::Point(x,y));
			temporary_route = new Avoid::ConnRef(router, Avoid::ConnEnd(temporary_junction), selectedConnEnd);
			router->processTransaction();
			activeconnend = true;
                        name_of_first_connref = oldname;
			previous_closestline = closestline; // Keep track of which wire we split here.
		}

	}else if((button == 1) && current_gate){
                must_save = true;
                select_closest_obstacle(x, y,false);
	}else if(button == 1){
                select_closest_obstacle(x, y,true);
	}

	clean_junction_positions();

        if(must_save){
                debug_save();
        }
	return;
	
}

void schematic_design::clean_junction_positions()
{
	for(const auto & connref : router->connRefs){
		std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
		Avoid::JunctionRef *j = endpoints.first.junction();
		if(j){
			if(!j->positionFixed()){
				while(router->shapeContainingPoint(j->position())){
					router->moveJunction(j, j->position() + Avoid::Point(10,10));
					router->processTransaction();
				}
			}
		}
		j = endpoints.second.junction();
		if(j){
			if(!j->positionFixed()){
				while(router->shapeContainingPoint(j->position())){
					router->moveJunction(j, j->position() + Avoid::Point(10,10));
					router->processTransaction();
				}
			}
		}
	}
	router->processTransaction();
}


void schematic_design::update_all_junctions()
{
	for(const auto & connref : router->connRefs){
		std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
		Avoid::JunctionRef *j = endpoints.first.junction();
		if(j){
			if(!j->positionFixed()){
				router->moveJunction(j, j->recommendedPosition());
			}
		}
		j = endpoints.second.junction();
		if(j){
			if(!j->positionFixed()){
				router->moveJunction(j, j->recommendedPosition());
			}
		}
	}
	router->processTransaction();
}

void schematic_design::refresh_highlighted_objects(double x, double y)
{
	
	x=int(x);
	y=int(y);
        lastx = x;
        lasty = y;
	scale_and_translate(&x,&y);
	if(x < 0){
		x = 0;
	}
	if(y < 0){
		y = 0;
	}
	if(x > 8000){
		x = 8000;
	}
	if(y > 8000){
		y = 8000;
	}
	

	// Add this for debugging output from libavoid
	// router->outputInstanceToSVG("libavoid_debugoutput");
 	select_closest_pin(x,y);
	double tmpx, tmpy;
	closestline = find_closest_line_segment(x,y,&tmpx, &tmpy);
	closestlinex = tmpx;
	closestliney = tmpy;
        
        trace_connref(closestline);

	
        closest_obstacle_is_gate = find_closest_obstacle(x, y);


	if(activeconnend){
		router->moveJunction(temporary_junction, Avoid::Point(x,y));
		router->processTransaction();
	}


	if(obstacle_is_selected){
                move_current_obstacle(x,y);
		clean_junction_positions();
		update_all_junctions();
	}
	
}


void schematic_design::register_verilog_module(struct module *m)
{
	struct module_inst *inst = m->insts;
	// For debugging
	// print_module(m);

	// FIXME: Load wirenames from verilog file!
	unsigned int wire_id = 0;
	std::string current_wirename = "UNDEFINED_DO_NOT_USE";
	
	while(inst){
		struct portconnection *port = inst->ports;
		gate * tmp = create_new_gate(inst->modulename, inst->instname);
		inst = inst->next;

		while(port){
			if(port->connection->type == IS_TILDE){
				tmp->toggleinv(tmp->get_connector_num(port->name));
			}
			port = port->next;
		}
	}
        router->processTransaction();
        struct schematic_statement *stmt = m->schematic_info;
	std::vector<Avoid::ConnEnd> startpin;
	Avoid::ConnEnd endpin;
	int startpinlevel = 0;
	Avoid::ConnRef *newconnref;
        while(stmt){
                switch(stmt->type){
                case SETPOS:
		{
                        printf("FOUND SETPOS %f %f\n", stmt->x1, stmt->y1);
                        if(!gate_names.count(stmt->name1)){
                                fprintf(stderr,"Error: An instance with name %s does not exist\n", stmt->name1);
                                abort();
                        }

                        gate *tmp = gate_names[stmt->name1];
                        tmp->move(stmt->x1, stmt->y1);
                        router->processTransaction();
                        printf("MOVING GATE %p\n",tmp);
		}
		break;
                case SETROT:
		{
                        printf("FOUND SETROT %f\n", stmt->x1);
                        if(!gate_names.count(stmt->name1)){
                                fprintf(stderr,"Error: An instance with name %s does not exist\n", stmt->name1);
                                abort();
                        }
                        gate *tmp = gate_names[stmt->name1];
			tmp->rotate(stmt->x1);
                        router->processTransaction();
                        printf("Rotating GATE %p\n",tmp);
		}
		break;
		case STARTROUTE:
		{
			printf("Found startroute %s.%s\n", stmt->name1, stmt->name2);
			if(gate_names.count(stmt->name1) == 0){
				printf("ERROR: Could not not locate this gate\n");
				break;
			}
			gate *tmp = gate_names[stmt->name1];
			int pin = tmp->get_connector_num(stmt->name2);
			startpin.clear();
			startpin.push_back(tmp->getConnEnd(pin));
			startpinlevel = 1;
			current_wirename = "unnamedwire" + std::to_string(wire_id++);
		}
		break;
		case ENDROUTE:
		{
			printf("Found endroute %s.%s\n", stmt->name1, stmt->name2);
			if(gate_names.count(stmt->name1) == 0){
				printf("ERROR: Could not not locate this gate\n");
				break;
			}
			gate *tmp = gate_names[stmt->name1];
			int pin = tmp->get_connector_num(stmt->name2);
			endpin= tmp->getConnEnd(pin);
			newconnref = new Avoid::ConnRef(router, startpin.back(), endpin);
			set_connref_name(newconnref, current_wirename);
		}
		break;
		case JUNCTION:
		{
		    
			Avoid::JunctionRef *j = new Avoid::JunctionRef(router, Avoid::Point(stmt->x1, stmt->y1));
			newconnref = new Avoid::ConnRef(router, startpin.back(), Avoid::ConnEnd(j));
			set_connref_name(newconnref, current_wirename);
			startpin.push_back(Avoid::ConnEnd(j));
			startpinlevel++;
		}
		break;
		case FIXEDJUNCTION:
		{
		    
			Avoid::JunctionRef *j = new Avoid::JunctionRef(router, Avoid::Point(stmt->x1, stmt->y1));
			j->setPositionFixed(true);
			newconnref = new Avoid::ConnRef(router, startpin.back(), Avoid::ConnEnd(j));
			set_connref_name(newconnref, current_wirename);
			startpin.push_back(Avoid::ConnEnd(j));
			startpinlevel++;
		}
		break;
		case POPJUNCTION:
			if(startpinlevel <= 0){
				printf("WARNING: No junction to pop!\n");
			}else{
				startpin.pop_back();
				startpinlevel--;
			}
			break;
		case SCHEMATICTEXT:
		{
			gate *textg = create_new_gate(".text", stmt->name1);
			textg->settext(stmt->name2);
			printf("New text gate with name %s\n", stmt->name1);
		}
		break;
		default:
			printf("Ignoring statement type %d\n", stmt->type);
			break;
                }
                stmt = stmt->next;
        }
        router->processTransaction();
}

#if 0
// Not working at the moment, commented out.
void schematic_design::replay_logfile(const char *filename)
{
	FILE *fp = fopen(filename,"r");
	char c;
	int arg;
	int x,y;
	printf("Replaying logfile:\n");
	while(fscanf(fp,"%c %d %d %d\n", &c,&arg,&x,&y) == 4){
		if(c == 'k'){
			printf("Replaying event %c (%c)\n", c,arg);
			if((arg == 'E') || (arg == 'T') || (arg == 't')){
				char buf[500];
				fgets(buf,490,fp);
				char *grr = &buf[0];
				while(*grr){
					if(*grr == '\n'){
						*grr = 0;
					}
					grr++;
				}
				printf("Sending string %s!\n", buf);
				handle_key(arg,x,y, &buf[0]);
			}else{
				handle_key(arg,x,y,NULL);
			}
		}else if(c == 'b'){
			printf("Replaying event %c (%d)\n", c,arg);
			handle_button(arg,x,y);
		}
	}
	fclose(fp);
#endif


schematic_design::schematic_design(const char *filename)
{

        current_maxgate = 0;
        closest_fixed_junction = NULL;
        current_gate_pin = NULL;
        current_gate = NULL;
        temporary_junction = NULL;
        temporary_route = NULL;
        activeconnend = false;

        cairo_translation_x = 0;
        cairo_translation_y = 0;
        cairo_scale_factor = 1;
        
        router = new Avoid::Router(Avoid::OrthogonalRouting);
        router->setTransactionUse(true);

	
        logfp = fopen("logfile.txt","w");
        
        if(filename){
                module_parse_info = NULL;
                FILE *fp = fopen(filename,"r");
                yyrestart(fp);
                start_token = START_WITH_LIMITED_PARSER;
                yyparse();
                fclose(fp);
                if(!module_parse_info){
                        printf("Error, could not read filename %s\n", filename);
                }else{
                        register_verilog_module(module_parse_info);
                }
        }

        router->setRoutingOption(Avoid::improveHyperedgeRoutesMovingJunctions, true);
        router->setRoutingPenalty(Avoid::segmentPenalty);
        router->setRoutingParameter(Avoid::segmentPenalty, 2);
	//   router->setRoutingParameter(Avoid::shapeBufferDistance, 15);
        router->setRoutingParameter(Avoid::idealNudgingDistance, 16);
        router->setRoutingPenalty(Avoid::fixedSharedPathPenalty, 9000);
        router->processTransaction();
	//   routes[0]->setRoutingCheckpoints(cp);
        router->processTransaction();
	//        router->outputInstanceToSVG("argh");
        current_gate_is_closest = true;
        obstacle_is_selected = false;

        closest_obstacle_is_gate = false;
	previous_closestline = NULL;
        
}
