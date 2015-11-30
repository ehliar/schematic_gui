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
#ifndef _DESIGN_HH
#define _DESIGN_HH

#define SCHEMATIC_VERSION "0.65"

class schematic_design {

private:

        gate *current_gate;
        gate *closest_gate;
        int current_pin;
        
        gate *selected_gate_pin;
        int selected_pin;
        bool current_gate_is_closest;
        bool obstacle_is_selected;
        static Avoid::ConnRef *route_with_closest_cp;

        bool  closest_obstacle_is_gate;
        
        bool firstmove;
        double previous_x;
        double previous_y;
        
        
        double cairo_translation_x;
        double cairo_translation_y;
        double cairo_scale_factor;
        gate *current_gate_pin;
        
        std::unordered_map<Avoid::ShapeRef *, gate *> shaperef_to_gates;
        std::unordered_map<std::string, gate *> gate_names;
        std::unordered_map<Avoid::ConnRef *, std::string> connref_names;

        unsigned int current_maxgate;

        std::string name_of_first_connref;


        std::unordered_set<gate *> gates;
                

        Avoid::Router *router;

        Avoid::ConnRef *closestline;
        Avoid::ConnRef *previous_closestline;
        double closestlinex, closestliney;
        double closestconnectorx, closestconnectory;
        Avoid::JunctionRef *closest_fixed_junction;
        Avoid::JunctionRef *temporary_junction;
        Avoid::ConnRef *temporary_route;
        bool activeconnend;
        Avoid::ConnEnd currentConnEnd;
        Avoid::ConnEnd selectedConnEnd;
        
        std::unordered_set<gate *> selected_gates;
        std::unordered_set<Avoid::JunctionRef *> selected_fixed_junctions;
        
        std::unordered_set<Avoid::ConnRef *> connected_connrefs;
        std::unordered_set<Avoid::JunctionRef *> connected_junctions;
        
        FILE *logfp;
        double lastx, lasty;

	void create_new_wirename(Avoid::ConnRef *connref);
	void debug_save();
	void clean_wirenames();
        void clean_junction_positions();
        bool is_same_wire(Avoid::ConnRef *connref1, Avoid::ConnRef *connref2);
        // FIXME: Move private stuff to private:
public:
        void update_viewport(double x, double y, double scaling) {
                cairo_translation_x = x;
                cairo_translation_y = y;
                cairo_scale_factor = scaling;
        }
        void synch_shaperefs();
        void trace_connref(Avoid::ConnRef *connref, bool startsearch=true, FILE *fp = NULL, int junctionlevel = 0);
        void set_connref_name_all(Avoid::ConnRef *connref, std::string thename);
        void set_connref_name(Avoid::ConnRef *connref, std::string thename);
        std::string get_connref_name(Avoid::ConnRef *connref);
        double check_distance(double x1, double y1, double x2, double y2);
        double check_distance(double x1, double y1, Avoid::JunctionRef *j);
        gate * create_new_gate(const char *filename, const char *instname = NULL);
        void add_gate(std::string gatename);
        void draw_junction(cairo_t *cr, Avoid::JunctionRef *j, bool is_ui);
        void draw_checkpoint(cairo_t *cr, Avoid::Checkpoint c);
        void draw_connref(cairo_t *cr, Avoid::ConnRef *connref, bool is_ui);
        void save_verilog(std::string filename);
        void draw_with_cairo(cairo_t *cr, bool is_ui);
        gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
        void save_svg(std::string filename);
        double find_distance(double x1, double y1, double x2, double y2, double x, double y, double *closestx, double *closesty);
        double find_closest_line_in_route(double x, double y, double *closestx, double *closesty, Avoid::ConnRef *r);
        void update_closest_fixed_junction(double x, double y, double *distance, Avoid::JunctionRef **bestsofar, Avoid::JunctionRef *junction);
        Avoid::JunctionRef *find_closest_fixed_junction(double x, double y);
        Avoid::ConnRef * find_closest_line_segment(double x, double y, double *xref, double *yref);
        void move_current_obstacle(double x, double y);
        gate * find_closest_gate(double x, double y, double *distance);
        void select_closest_pin(double x, double y);
        void remove_unused_junctions();
        void add_checkpoint_to_closest_line(double x, double y);
        void remove_connref_and_fixed_junctions(Avoid::ConnRef *connref);
        void remove_closest_object(double x, double y);
        void selection_add();
        void scale_and_translate(double *x, double *y);
        void create_new_text_and_attach(double x, double y, const char *edittext);
        bool handle_key(int keyval);
        void select_closest_obstacle(double x, double y, bool select);
        bool find_closest_obstacle(double x, double y);
        void handle_button(int button, double x, double y);
        void update_all_junctions();
        void refresh_highlighted_objects(double x, double y);
        void register_verilog_module(struct module *m);
        void select_all_objects();
//        void replay_logfile(const char *filename);

        schematic_design(const char *filename = NULL);
};

#endif
