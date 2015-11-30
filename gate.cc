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
#include "gate.hh"
#include <libavoid/libavoid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include "verilog-subset.h"

// FIXME: Rename to object, or something similar!

Avoid::Polygon gate::rotate_polygon(Avoid::Polygon p, int times, double middle_x, double middle_y)
{
	//	   std::vector<Avoid::Point> ps = p.ps;
	if(p.ps.size() == 0){
		return p;
	}

	for(std::vector<Avoid::Point>::iterator it = p.ps.begin(); it < p.ps.end(); it++){
		int i;
		for(i=0; i < times; i++){
			double x = it->x - middle_x-this->x;
			double y = it->y - middle_y-this->y;

			// printf("%lf x %lf ->", it->x, it->y);
			it->x = y + middle_y +this->x;
			it->y = -x + middle_x +this->y;
			// printf("  %lf x %lf\n", it->x, it->y);

			//                           it->x = x+middle_x + 10;
			//                           it->y = y+middle_y;
		}
	}
	return p;
        
}

void gate::rotate(int times)
{
	unsigned int i;
	double tmp;
	while(times > 0){
		Avoid::Polygon p = shapeRef->polygon();

		rotation = (rotation + 1) % 4;

		// printf("ROTATING poly %s (%f %f)!\n", instancename.c_str(), middle_x, middle_y);
		router->moveShape(shapeRef, rotate_polygon(p,1, middle_x, middle_y));
		for(i=0; i < xpoly.size(); i++){
			if(xpoly[i] < 0){
				continue;
			}
			double x = xpoly[i] - middle_x;
			double y = ypoly[i] - middle_y;
			xpoly[i] = y + middle_y;
			ypoly[i] = -x + middle_x;
		}
		for(i=0; i < xpin.size(); i++){
			double x = xpin[i] - middle_x;
			double y = ypin[i] - middle_y;
			xpin[i] = y + middle_y;
			ypin[i] = -x + middle_x;
		}
		tmp = middle_x;
		middle_x = middle_y;
		middle_y = tmp;
		router->processTransaction();

		//                move(-middle_x, -middle_y);
		shapeRef->transformConnectionPinPositions(Avoid::TransformationType_CW270);
		//                move(middle_x, middle_y);
		times--;
		router->processTransaction();
	}
        
}
void gate::add_avoid_feature(state_t state, std::vector<double> xcoords, std::vector<double> ycoords)
{
	unsigned int i;
	if(state == AVOID){
		Avoid::Polygon shapePoly(xcoords.size());
		for(i=0; i < xcoords.size(); i++){
			shapePoly.ps[i] = Avoid::Point(xcoords[i], ycoords[i]);
			//      printf("shapepoly %lf %lf\n", xcoords[i], ycoords[i]);
			maxx = (maxx < xcoords[i]) ? xcoords[i] : maxx;
			maxy = (maxy < ycoords[i]) ? ycoords[i] : maxy;
		}
		shapeRef = new Avoid::ShapeRef(router, shapePoly);
	}else if(state == POLYGON){
		double max_x=0, max_y=0;
		for(i=0; i < xcoords.size(); i++){
			if(xcoords[i] > max_x){
				max_x = xcoords[i];
			}
			if(ycoords[i] > max_y){
				max_y = ycoords[i];
			}
		}    
		xpoly = xcoords;
		ypoly = ycoords;
		middle_x = max_x/2;
		middle_y = max_y/2;
	}else if(state == CONNECTORPIN) {
		for(i=0; i < xcoords.size(); i++){
			maxconnector++;
			Avoid::ShapeConnectionPin *pin = new Avoid::ShapeConnectionPin(shapeRef, maxconnector, xcoords[i]/maxx,ycoords[i]/maxy, 10.0, Avoid::ConnDirNone);
			pin->setExclusive(false);

			xpin.push_back(xcoords[i]);
			ypin.push_back(ycoords[i]);
			invpin.push_back(false);


		}
	}
  

}
void gate::toggleinv(unsigned int pinno)
{
	if(pinno >= invpin.size()){
		abort();
	}
	invpin[pinno] = !invpin[pinno];
}

bool gate::isinverted(unsigned int pinno)
{
	if(pinno >= invpin.size()){
		abort();
	}
	return invpin[pinno];
}

void gate::settext(std::string newtext)
{
	text = newtext;
	checktextsize();

	Avoid::Rectangle test(Avoid::Point(x,y), textwidth+10, textheight+10);

	if(!shapeRef){
		shapeRef = new Avoid::ShapeRef(router, test);

		new Avoid::ShapeConnectionPin(shapeRef, ++maxconnector, Avoid::ATTACH_POS_LEFT, Avoid::ATTACH_POS_CENTRE, 1, Avoid::ConnDirNone);
		new Avoid::ShapeConnectionPin(shapeRef, ++maxconnector, Avoid::ATTACH_POS_RIGHT, Avoid::ATTACH_POS_CENTRE, 1, Avoid::ConnDirNone);
		new Avoid::ShapeConnectionPin(shapeRef, ++maxconnector, Avoid::ATTACH_POS_CENTRE, Avoid::ATTACH_POS_RIGHT, 1, Avoid::ConnDirNone);
		new Avoid::ShapeConnectionPin(shapeRef, ++maxconnector, Avoid::ATTACH_POS_CENTRE, Avoid::ATTACH_POS_LEFT, 1, Avoid::ConnDirNone);
		//           maxconnector++;
		//           new Avoid::ShapeConnectionPin(shapeRef, maxconnector, 0.5,0.0,0.0, Avoid::ConnDirUp);
		//           maxconnector++;
		//           new Avoid::ShapeConnectionPin(shapeRef, maxconnector, 0.5,1.0,0.0, Avoid::ConnDirDown);
		//          new Avoid::ShapeConnectionPin(shapeRef, maxconnector, 0,0,0,Avoid::ConnDirNone);

          
                
	}else{
		router->moveShape(shapeRef, test);
	}

	xpin.clear();
	ypin.clear();
        
	xpin.push_back(-textwidth/2);  // left
	ypin.push_back(0);
	xpin.push_back(textwidth/2);   // right
	ypin.push_back(0);
	xpin.push_back(0);
	ypin.push_back(textheight/2);  // down
	xpin.push_back(0);
	ypin.push_back(-textheight/2); // up
	invpin.push_back(false);
	invpin.push_back(false);
	invpin.push_back(false);
	invpin.push_back(false);


	ports.clear();
	portnames.clear();

	ports["l"] = 1;
	ports["r"] = 2;
	ports["d"] = 3;
	ports["u"] = 3;
	portnames[1] = "l";
	portnames[2] = "r";
	portnames[3] = "d";
	portnames[4] = "u";
	maxconnector = 4;
	
}
std::string gate::get_connector_name(int num)
{
	
        return portnames[num];
}
int gate::get_connector_num(std::string name)
{
	if(!ports.count(name)){
		fprintf(stderr, "ERROR: %s was not found.\n", name.c_str());
		abort();
		return -1;
	}

	return ports[name]-1; // FIXME: Fix ugly offset here for good
}

gate::gate(Avoid::Router *r, struct module *m)
{
	std::vector <double> xcoords, ycoords;

	rotation = 0;
	x=0;
	y=0;
	maxconnector = 0;
	router = r;

	name = m->modulename;

	struct module_port *port = m->ports;
	istext = 0;

	while(port){
		verilog_ports.emplace(port->name);
		// FIXME: Direction, etc.
		port = port->next;
	}

	struct schematic_statement *stmt;
	stmt = m->schematic_info;
	while(stmt){
		//	  printf("Parsing statement %d\n", stmt->type);
		switch(stmt->type){
		case SETPOS:
			// printf("Ignoring setgatepos\n");
			break; 
		case BOUNDINGBOX:
			xcoords.clear(); ycoords.clear();
			xcoords.push_back(0); ycoords.push_back(0);
			xcoords.push_back(stmt->x1); ycoords.push_back(0);
			xcoords.push_back(stmt->x1); ycoords.push_back(stmt->y1);
			xcoords.push_back(0); ycoords.push_back(stmt->y1);
			add_avoid_feature(AVOID, xcoords, ycoords);

			middle_x = stmt->x1/2;
			middle_y = stmt->y1/2;
			break;
		case POLYGONSTART:
			xpoly.push_back(-1); ypoly.push_back(-1);
			break;
		case LINESTART:
			break;
		case COORD:
			xpoly.push_back(stmt->x1); ypoly.push_back(stmt->y1);
			break;
		case POLYGONEND:
			xpoly.push_back(-2); ypoly.push_back(-2);
			break;
		case LINEEND:
			xpoly.push_back(-3); ypoly.push_back(-3);
			break;
		case ARC:
			xpoly.push_back(-4); ypoly.push_back(stmt->x2);
			xpoly.push_back(stmt->x1); ypoly.push_back(stmt->y1);
			break;
		case SYMBOLTEXT:
			symboltext.push_back(stmt->name1);
			xpoly.push_back(-5); ypoly.push_back(symboltext.size()-1);
			xpoly.push_back(stmt->x1); ypoly.push_back(stmt->y1);
			break;
		case CONNECTOR:
		{
			//FIXME: Boundingbox MUST be set first! (or rather, shaperef must be created)
			maxconnector++;
			Avoid::ShapeConnectionPin *pin = new Avoid::ShapeConnectionPin(shapeRef, maxconnector, stmt->x1 / maxx,stmt->y1/maxy, 10.0, Avoid::ConnDirNone);
			pin->setExclusive(false);
			// printf("Adding %s to ports\n", stmt->name1);
			if(verilog_ports.count(stmt->name1) == 0){
				// FIXME: Fail gracefully here?
				fprintf(stderr, "    ERROR: port does not exist in verilog definition of module!\n");
				abort();
			}
			ports[stmt->name1] = maxconnector;
			portnames[maxconnector] = stmt->name1;
			// FIXME: Check that connector name exists!
			
			xpin.push_back(stmt->x1);
			ypin.push_back(stmt->y1);
			invpin.push_back(false);
		}
		break;

		default:
			break;
			//printf("UNKNOWN $schematic_* statement, id %d\n", stmt->type);
		}
		stmt = stmt->next;
	}


}

gate::gate(Avoid::Router *r, const char *filename)
{
	std::vector <double> xcoords, ycoords;
	router = r;

	rotation = 0;
	maxconnector = 0;
	x = 0;
	y = 0;
	shapeRef = NULL;
	istext = false;
	if(!strcmp(filename, ".text")){
		istext = true;
		textwidth = 0;
		textheight = 0;
		settext("");
		middle_x = 0;
		middle_y = 0;

		return;
	}

	maxx=0;
	maxy=0;

	fprintf(stderr,"Error, we should never get here!\n");
	abort();
	
	FILE * fp = fopen(filename,"r");
	char buf[100];
	buf[99] = 0;
	state_t state;
	highlightpin = -1;
	while(fgets(buf, 99, fp) != 0){
		double x, y;
		if(sscanf(buf, "%lf, %lf", &x, &y) == 2){
			//			printf("Pushing back %f %f\n", x,y);
			xcoords.push_back(x);
			ycoords.push_back(y);
		}else if(!strncmp("avoid", buf, 5)){
			state = AVOID;
			xcoords.clear();
			ycoords.clear();
		}else if(!strncmp("polygon", buf, 7)){
			add_avoid_feature(state, xcoords, ycoords);
			state = POLYGON;
			xcoords.clear();
			ycoords.clear();
		}else if(!strncmp("connector", buf, 9)){
			add_avoid_feature(state, xcoords, ycoords);
			state = CONNECTORPIN;
			xcoords.clear();
			ycoords.clear();
		}
	}

	add_avoid_feature(state, xcoords, ycoords);

	connectioncount = 0;

	//   Avoid::Polygon shapePoly(4);
	//   shapePoly.ps[0] = Avoid::Point(0,0);
	//   shapePoly.ps[1] = Avoid::Point(30,30);
	//   shapePoly.ps[2] = Avoid::Point(30,70);
	//   shapePoly.ps[3] = Avoid::Point(0,100);
	//   shapeRef = new Avoid::ShapeRef(r, shapePoly);
	//  router->moveShape(shapeRef, 0, 0);
  
}


Avoid::ConnEnd gate::getConnEnd(int id)
{
	if(id > maxconnector){
		fprintf(stderr, "connector id %d too large in gate %s (%s)\n", id, instancename.c_str(),name.c_str());
		abort();
	}
	return Avoid::ConnEnd(shapeRef,id+1);
}

Avoid::ConnRef *gate::connect(int id, gate *other, int otherid)
{
	if(connectioncount == 0){
		connectioncount++;
	}
	Avoid::ConnEnd newSrcPt(shapeRef, id);
	Avoid::ConnEnd newDstPt(other->shapeRef, otherid);

	Avoid::ConnRef *tmp = new Avoid::ConnRef(router, newSrcPt, newDstPt);
	router->processTransaction();
	//    add_new_route(tmp);
	return tmp;


}

// FIXME: Don't look merely at the center distance, also look at the
// distance to the nearest polygon?
double gate::getdistance(double x, double y)
{
	return sqrt((x-this->x-middle_x)*(x-this->x-middle_x)+(y-this->y-middle_y)*(y-this->y-middle_y));
}

double gate::getdistance_pin(int pin, double x, double y)
{
	if(pin >= maxconnector){
		abort();
	}
	return sqrt((x-xpin[pin]-this->x)*(x-xpin[pin]-this->x)+(y-ypin[pin]-this->y)*(y-ypin[pin]-this->y));
  
}
double gate::getclosestpin(double x, double y, int *pin)
{
	int i;
	int currpin = 0;
  
	double distance = getdistance_pin(0,x,y);
	for(i=1; i < maxconnector; i++){
		double tmp;
		tmp = getdistance_pin(i,x,y);
		if(tmp < distance){
			currpin = i;
			distance = tmp;
		}
	}
	if(!pin){
		abort();
	}
	*pin = currpin;
	return distance;
}


void gate::sethighlight(int pin)
{
	abort(); // Should not be used any longer
	highlightpin = pin;
}
void gate::clearhighlight()
{
	abort();
	highlightpin = -1;
}

void gate::move(double x, double y)
{
	router->moveShape(shapeRef, -this->x, -this->y);
	this->x = x;
	this->y = y;
	router->moveShape(shapeRef, x, y);
}

void gate::movedelta(double dx, double dy)
{
	x += dx;
	y += dy;
  
	router->moveShape(shapeRef, dx,dy);
}

void gate::checktextsize()
{
	cairo_surface_t *surface;
	cairo_t *cr;
	cairo_text_extents_t extents;
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 10,10);
	cr = cairo_create(surface);
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 20.0);
	cairo_text_extents(cr, text.c_str(), &extents);
	textwidth = extents.width;
	textheight = extents.height;
	text_xbearing = extents.x_bearing;
	text_ybearing = extents.y_bearing;
	cairo_surface_destroy(surface);
	cairo_destroy(cr);
        
}

void gate::draw(cairo_t *cr, bool is_ui)
{



	if(istext){

		cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
					CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size (cr, 20.0);

		if(is_ui){
			cairo_save(cr);
			cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
			cairo_move_to (cr, x, y);
			cairo_rotate(cr, rotation*M_PI/2);
			cairo_rel_move_to(cr, -textwidth/2-5, -textheight/2-5);
			cairo_rel_line_to (cr, textwidth+10, 0);
			cairo_rel_line_to (cr, 0, textheight+10);
			cairo_rel_line_to (cr, -textwidth-10, 0);
			cairo_close_path (cr);
			
			cairo_fill(cr);
			cairo_restore(cr);
		}

		cairo_save(cr);
		//		cairo_move_to (cr, x-textwidth/2-5 -text_xbearing+5,y-textheight/2-5-text_ybearing + 5);
		cairo_move_to (cr, x,y);
		cairo_rotate(cr, rotation*M_PI/2);
		cairo_rel_move_to (cr, -textwidth/2-5 -text_xbearing+5,-textheight/2-5-text_ybearing + 5);		
		cairo_show_text (cr, text.c_str());
		cairo_stroke(cr);
		cairo_restore(cr);
                
		return;
	}
        

	bool needs_moveto = true;
	cairo_set_line_width(cr,3);

	for(unsigned int i=0; i < xpoly.size(); i++){
		if(xpoly[i] == -1){
			cairo_new_path(cr);
		}else if(xpoly[i] == -2){
			cairo_close_path(cr);
			cairo_stroke(cr);
			needs_moveto = true;
		}else if(xpoly[i] == -3){
			cairo_stroke(cr);
			needs_moveto = true;
		}else if(xpoly[i] == -4){
			// FIXME: Bounds checking!
			cairo_arc(cr, xpoly[i+1]+x, ypoly[i+1]+y, ypoly[i], 0, 2*M_PI);
			i++;
			cairo_stroke(cr);
			needs_moveto = true;
		}else if(xpoly[i] == -5){
			const char * tmp = symboltext[ypoly[i]].c_str();
                        
			cairo_move_to(cr, xpoly[i+1]+x, ypoly[i+1]+y);
			cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
						CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size (cr, 20.0);
			cairo_text_extents_t extents;
			cairo_text_extents(cr, tmp, &extents);
			cairo_move_to(cr, xpoly[i+1]+x-extents.width/2-extents.x_bearing, ypoly[i+1]+y-extents.height/2-extents.y_bearing);
			cairo_show_text(cr, tmp);
			i++;
			needs_moveto = true;
		}else if(needs_moveto){
			cairo_move_to(cr, xpoly[i]+x,ypoly[i]+y);
			needs_moveto = false;
		}else{
			cairo_line_to(cr, xpoly[i]+x,ypoly[i]+y);
		}
	}
	cairo_close_path(cr);
                
	/* stroke the rectangle's path with the chosen color so it's actually visible */
	cairo_stroke(cr);
	for(unsigned int i=0;i < xpin.size(); i++){
		if(invpin[i]){
			cairo_save(cr);
			cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
			cairo_arc(cr, xpin[i]+x, ypin[i]+y, 10,0,2*M_PI);
			cairo_fill(cr);
			cairo_restore(cr);
			cairo_arc(cr, xpin[i]+x, ypin[i]+y, 10,0,2*M_PI);
			cairo_stroke(cr);
                   
		}
	}

}


void gate::get_connector_coords(int pin, double *x, double *y)
{
	*x = xpin[pin]+this->x;
	*y = ypin[pin]+this->y;

}


Avoid::ConnRef * gate::get_connref_by_pin(unsigned int pin)
{
	Avoid::ConnRefList l = shapeRef->attachedConnectors();
	for(Avoid::ConnRef * connref : l){
		std::pair<Avoid::ConnEnd, Avoid::ConnEnd> endpoints = connref->endpointConnEnds();
		if(endpoints.first.shape() == shapeRef){
			if(pin == endpoints.first.pinClassId()){
				return connref;
			}
		}
		if(endpoints.second.shape() == shapeRef){
			if(pin == endpoints.second.pinClassId()){
				return connref;
			}
		}
	}
	return NULL;
		
}
