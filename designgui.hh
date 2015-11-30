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
#ifndef _DESIGNGUI_HH
#define _DESIGNGUI_HH

class schematic_gui {

private:
        schematic_design *thedesign;
        GtkWidget *window;
        GtkWidget *da;
        GtkWidget *helpwindow;

	double lastx, lasty;
        gint width, height;
        double cairo_translation_x;
        double cairo_translation_y;
        double cairo_scale_factor;

        std::string current_filename;

	void set_filename(const char *filename);

        void create_help_widget();

        static gboolean do_hide_help(GtkWidget *widget, GdkEvent *event, gpointer user_data);
        static gboolean do_show_help(GtkWidget *widget, gpointer user_data);

        static std::string do_dialog(GtkWidget *dialog);
	
        static gboolean do_new(GtkWidget *widget, gpointer user_data);
        static gboolean do_open(GtkWidget *widget, gpointer user_data);
        static gboolean do_recover(GtkWidget *widget, gpointer user_data);
        static gboolean do_save(GtkWidget *widget, gpointer user_data);
        static gboolean do_save_as(GtkWidget *widget, gpointer user_data);
        static gboolean do_about(GtkWidget *widget, gpointer user_data);
        static gboolean do_export(GtkWidget *widget, gpointer user_data);
  
        static gboolean configure_event(GtkWidget *widget, GdkEvent *ev, gpointer data);
        static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
        static gboolean press_key(GtkWidget *widget, GdkEvent *ev, gpointer user_data);
        static gboolean press_button(GtkWidget *widget, GdkEvent *ev, gpointer user_data);
        static gboolean release_button(GtkWidget *widget, GdkEvent *ev, gpointer user_data);
        static gboolean motion_event(GtkWidget *widget, GdkEvent *ev, gpointer user_data);
        static gboolean scroll_event(GtkWidget *widget, GdkEvent *ev, gpointer user_data);
        void moveviewport(int dx, int dy);
        void zoomin();
        void zoomout();

        bool print_preview_enabled;
        
public:
	void set_filename(std::string filename);
	schematic_gui(schematic_design *design, const char *filename);
};


        
#endif
