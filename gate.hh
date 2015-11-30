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
#include <libavoid/libavoid.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class gate {
private:
        double x;
        double y;
        double middle_x, middle_y;
        
        int maxconnector;
        double maxx, maxy;


        typedef enum {UNDEFINED, AVOID, POLYGON, CONNECTORPIN} state_t ;
        Avoid::Router *router;
        std::vector<double> xpoly;
        std::vector<double> ypoly;
        std::vector<std::string> symboltext;
        int connectioncount;
        int highlightpin;

        std::vector<double> xpin;
        std::vector<double> ypin;
        std::vector<bool> invpin;

        std::vector<Avoid::ConnRef *> connrefs;
        std::vector<int> connref_pins;
        std::vector<Avoid::JunctionRef *> junctions;


        int rotation;

        double textwidth;
        double textheight;
        double text_xbearing;
        double text_ybearing;
	std::string text;

        std::unordered_map<std::string, int> ports;
        std::unordered_map<int, std::string> portnames;
        std::unordered_set<std::string> verilog_ports;

        std::string name;
	std::string instancename;


public:
	void setinstancename(std::string n) {instancename = n;}
	std::string getinstancename() {return instancename;}
        unsigned int maxpins() {return maxconnector;}
        int get_connector_num(std::string name);
        std::string get_connector_name(int num);
        void toggleinv(unsigned int pinno);
	bool isinverted(unsigned int pin);
        bool istext;
        void checktextsize();
        void settext(std::string newtext);
	std::string gettext() { return text;}
        Avoid::Polygon rotate_polygon(Avoid::Polygon p, int times, double middle_x, double middle_y);
        Avoid::ShapeRef *shapeRef;
        void rotate(int x);
        int  getrotation() { return rotation; }
        double getclosestpin(double x, double y, int *pin);
        void get_connector_coords(int pin, double *x, double *y);
        Avoid::ConnEnd getConnEnd(int id);
        Avoid::ConnRef * get_connref_by_pin(unsigned int pin)  ;
        void move(double x, double y);
        void movedelta(double x, double y);
        void draw(cairo_t *cr, bool is_ui);
        Avoid::ConnRef *connect(int id, gate *other, int otherid);
        void add_avoid_feature(state_t state, std::vector<double> xcoords, std::vector<double> ycoords);
        gate(Avoid::Router *r, const char *filename);
        gate(Avoid::Router *r, struct module *m);

        double getdistance(double x, double y);
        double getdistance_pin(int pin, double x, double y );

	Avoid::Point getpos() { return Avoid::Point(x,y); }

        void sethighlight(int pin);
        void clearhighlight();
        std::string getname() { return name; }
};


void add_new_route(Avoid::ConnRef *r);
