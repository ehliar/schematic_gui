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
#include "gate.hh"
#include <string.h>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include <cairo-svg.h>
#include "verilog-subset.h"
#include "verilog-subset.tab.h"
#include "verilog-subset.yy.h"

#include "design.hh"
#include "designgui.hh"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

gboolean schematic_gui::draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
        schematic_gui *sg = (schematic_gui *) user_data;
        sg->thedesign->draw_with_cairo(cr,!sg->print_preview_enabled);
        return FALSE;
}

// Show dialog, destroy dialog, return selected filename (if any), otherwise empty string.
std::string schematic_gui::do_dialog(GtkWidget *dialog)
{
	std::string retstring;
	gint res = gtk_dialog_run(GTK_DIALOG(dialog));
	if(res == GTK_RESPONSE_ACCEPT){
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		
		if(!filename){
			retstring = "";
		}else{
			retstring = filename;
			free(filename);
		}
		
	}else{
		retstring = "";
	}
	
	gtk_widget_destroy(dialog);
	return retstring;
}

gboolean schematic_gui::do_export(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	std::string svgname = sg->current_filename;

	std::size_t suffix = svgname.rfind(".v");
	
	if(svgname.length() == 0){
		svgname = "";
	}else if(suffix == std::string::npos){
		svgname = svgname + ".svg";
	}else if(suffix == svgname.length() - 2){
		svgname.replace(suffix,2,".svg");
	}else{
		svgname = svgname + ".svg";
	}

		
	GtkWidget *chooser = gtk_file_chooser_dialog_new("Save schematic as...",
							 GTK_WINDOW(sg->window),
							 GTK_FILE_CHOOSER_ACTION_SAVE, 
							 "_Cancel", GTK_RESPONSE_CANCEL,
							 "_Save", GTK_RESPONSE_ACCEPT,
							 NULL);
	if(svgname.length() > 0){
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser), svgname.c_str());
	}

	std::string newfile = do_dialog(chooser);
	if(newfile.length() == std::string::npos){
		return FALSE;
	}

	sg->thedesign->save_svg(newfile);

	return FALSE;
}

gboolean schematic_gui::do_save_as(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	GtkWidget *chooser = gtk_file_chooser_dialog_new("Save schematic as...",
							 GTK_WINDOW(sg->window),
							 GTK_FILE_CHOOSER_ACTION_SAVE, 
							 "_Cancel", GTK_RESPONSE_CANCEL,
							 "_Save", GTK_RESPONSE_ACCEPT,
							 NULL);
	if(sg->current_filename.length() > 0){
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser), sg->current_filename.c_str());
	}
	
	std::string newfile = do_dialog(chooser);
	if(newfile.length() == std::string::npos){
		return FALSE;
	}
	
	sg->set_filename(newfile);
	sg->thedesign->save_verilog(newfile.c_str());

	return FALSE;


}

gboolean schematic_gui::do_save(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	if(sg->current_filename.length() == 0){
		return do_save_as(widget, user_data);
	}else{
		sg->thedesign->save_verilog(sg->current_filename.c_str());
	}
	return FALSE;
}

gboolean schematic_gui::do_new(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	delete sg->thedesign; // FIXME: Deletion handler!
	sg->thedesign = new schematic_design(NULL);
	sg->current_filename = "";
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
	return FALSE;
}

gboolean schematic_gui::do_open(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	GtkWidget *chooser = gtk_file_chooser_dialog_new("Open schematic file...",
							 GTK_WINDOW(sg->window),
							 GTK_FILE_CHOOSER_ACTION_OPEN, 
							 "_Cancel", GTK_RESPONSE_CANCEL,
							 "_Open", GTK_RESPONSE_ACCEPT,
							 NULL);

	if(sg->current_filename.length() > 0){
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser), sg->current_filename.c_str());
	}

	std::string newfile = do_dialog(chooser);
	if(newfile.length() == 0){
		return FALSE;
	}
	delete sg->thedesign;

	sg->thedesign = new schematic_design(newfile.c_str());
	sg->set_filename(newfile);
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
	return FALSE;
}

gboolean schematic_gui::do_recover(GtkWidget *widget, gpointer user_data)
{
	schematic_gui *sg = static_cast<schematic_gui *>(user_data);
	GtkWidget *chooser = gtk_file_chooser_dialog_new("Open schematic file...",
							 GTK_WINDOW(sg->window),
							 GTK_FILE_CHOOSER_ACTION_OPEN, 
							 "_Cancel", GTK_RESPONSE_CANCEL,
							 "_Open", GTK_RESPONSE_ACCEPT,
							 NULL);

	if(sg->current_filename.length() > 0){
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(chooser), sg->current_filename.c_str());
	}
	std::string recoverdir = getenv("HOME");
	recoverdir = recoverdir + "/.schematic_gui/";
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(chooser), recoverdir.c_str());

	std::string newfile = do_dialog(chooser);
	if(newfile.length() == 0){
		return FALSE;
	}
	delete sg->thedesign;

	sg->thedesign = new schematic_design(newfile.c_str());
	sg->set_filename(std::string("Recover.v"));
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
	return FALSE;
}

gboolean schematic_gui::do_about(GtkWidget *widget, gpointer user_data)
{
	const gchar *authors[2] = { "Andreas Ehliar", NULL};
	GtkWidget *about = gtk_about_dialog_new();
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "http://github.com/ehliar/schematic_gui");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), "http://github.com/ehliar/schematic_gui");
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "schematic_gui");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(about), SCHEMATIC_VERSION);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(about), "(C) Andreas Ehliar 2015");
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG(about), GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(about), authors);
	gtk_dialog_run(GTK_DIALOG(about));
	gtk_widget_destroy(about);

        return FALSE;
	
}

gboolean schematic_gui::press_key(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
	GdkEventKey *k = (GdkEventKey *) ev;
        schematic_gui *sg = (schematic_gui *) user_data;
//	GdkModifierType modifiers = gtk_accelerator_get_default_mod_mask();
	
	// printf("Key %d pressed\n", k->keyval);
        bool must_invalidate = true;


        if(k->keyval == GDK_KEY_Right){
                sg->moveviewport(16,0);
	}else if(k->keyval == GDK_KEY_Left){
                sg->moveviewport(-16,0);
        }else if(k->keyval == GDK_KEY_Up){
                sg->moveviewport(0,-16);
	}else if(k->keyval == GDK_KEY_Down){
                sg->moveviewport(0,16);
        }else if(k->keyval == 'p'){
                sg->print_preview_enabled = !sg->print_preview_enabled;
	}else if(k->keyval == 's'){
		if(k->state & GDK_CONTROL_MASK){
			do_save(NULL, user_data);
		}
        }else{
                must_invalidate = sg->thedesign->handle_key(k->keyval);
        }

        

        if(must_invalidate){
                gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
        }
		
//	Avoid::JunctionRef *j = new Avoid::JunctionRef(router, Avoid::Point(closestlinex, closestliney));
//	junctions[current_maxjunction++] = j;
	
	return FALSE;

}

gboolean schematic_gui::press_button(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
        schematic_gui *sg = (schematic_gui *) user_data;
	GdkEventButton *b = (GdkEventButton *)  ev;
	// printf("Button %d pressed at %lf, %lf\n",b->button, b->x, b->y);
	sg->thedesign->handle_button(b->button,b->x,b->y);

	// Probably need to refresh at this point...
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);

	return FALSE;
}

gboolean schematic_gui::release_button(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
//        schematic_gui *sg = (schematic_gui *) user_data;
	// printf("Button released\n");
	return FALSE;
}

gboolean schematic_gui::motion_event(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
        schematic_gui *sg = (schematic_gui *) user_data;
	GdkEventMotion *motion = (GdkEventMotion *) ev;
	double x = motion->x;
	double y = motion->y;
	sg->lastx = x;
	sg->lasty = y;
	sg->thedesign->refresh_highlighted_objects(x,y);
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
	return FALSE;
}


gboolean schematic_gui::configure_event(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
        schematic_gui *sg = (schematic_gui *) user_data;
        // printf("RESIZED!\n");
        gtk_window_get_size(GTK_WINDOW(sg->window),&sg->width,&sg->height);
        // printf("New size is %d %d\n",sg->width,sg->height);
        return FALSE;
}

gboolean schematic_gui::scroll_event(GtkWidget *widget, GdkEvent *ev, gpointer user_data)
{
	GdkEventScroll *s = (GdkEventScroll *) ev;
        schematic_gui *sg = (schematic_gui *) user_data;

	if(s->direction == GDK_SCROLL_UP){
	  sg->zoomin();
	}else if(s->direction == GDK_SCROLL_DOWN){
	  sg->zoomout();
	}
	// printf("SCROLLING!\n");
	gdk_window_invalidate_rect(gtk_widget_get_window(GTK_WIDGET(widget)), NULL, 1);
        return FALSE;
}

void schematic_gui::moveviewport(int dx, int dy)
{
        // printf("Move viewport\n");
        cairo_translation_x += dx;
        cairo_translation_y += dy;
        thedesign->update_viewport(cairo_translation_x, cairo_translation_y, cairo_scale_factor);
}

void schematic_gui::zoomin()
{
        // printf("Middle is %d x %d\n", width/2, height/2);
        cairo_translation_x -= lastx / cairo_scale_factor;
        cairo_translation_y -= lasty / cairo_scale_factor;
	
        cairo_scale_factor = cairo_scale_factor * 2;
        cairo_translation_x += lastx / cairo_scale_factor;
        cairo_translation_y += lasty / cairo_scale_factor;
        // printf("New translation is %f %f\n", cairo_translation_x, cairo_translation_y);
        thedesign->update_viewport(cairo_translation_x, cairo_translation_y, cairo_scale_factor);
}

void schematic_gui::zoomout()
{
        cairo_translation_x -= lastx / cairo_scale_factor;
        cairo_translation_y -= lasty / cairo_scale_factor;
        cairo_scale_factor = cairo_scale_factor * 0.5;
        cairo_translation_x += lastx / cairo_scale_factor;
        cairo_translation_y += lasty / cairo_scale_factor;
        // printf("New translation is %f %f\n", cairo_translation_x, cairo_translation_y);
        thedesign->update_viewport(cairo_translation_x, cairo_translation_y, cairo_scale_factor);
}

void schematic_gui::set_filename(std::string filename)
{
	current_filename = filename;
	std::string newtitle = "schematic_gui: ";
	newtitle = newtitle + current_filename;
	gtk_window_set_title(GTK_WINDOW(window), newtitle.c_str());
}

void schematic_gui::create_help_widget()
{
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *closebutton = gtk_button_new_with_label("Close help window");
        
        GtkWidget *textview = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
        GtkTextBuffer *buffer;
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

        const char *helptext; // Jump through hoop as we can't read
                              // test into a const gchar * using g_file_get_contents
        gchar *helptextfromfile;
        gsize helplength;
        if(!g_file_get_contents(PREFIX "/share/schematic_gui/schematic_gui.help", &helptextfromfile, &helplength, NULL)){
                helptext = "ERROR: Could not open help file (did you run make install?)";
                helplength = strlen(helptext);
        }else{
                helptext = helptextfromfile;
        }
        gtk_text_buffer_set_text(buffer, helptext, helplength);
        gtk_widget_set_can_focus(textview, FALSE);
        gtk_widget_set_can_focus(closebutton, FALSE);
        g_signal_connect (closebutton, "button-press-event", G_CALLBACK (do_hide_help), this);

	gtk_box_pack_start(GTK_BOX(vbox), textview, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), closebutton, FALSE, FALSE, 2);
        helpwindow = vbox;
}

gboolean schematic_gui::do_hide_help(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
        schematic_gui *sg = static_cast<schematic_gui *>(user_data);
        gtk_widget_hide(sg->helpwindow);
        
        return FALSE;
}

gboolean schematic_gui::do_show_help(GtkWidget *widget, gpointer user_data)
{
        schematic_gui *sg = static_cast<schematic_gui *>(user_data);
        gtk_widget_show(sg->helpwindow);
        
        return FALSE;
}

gboolean schematic_gui::do_show_tutorial(GtkWidget *widget, gpointer user_data)
{
	if(!gtk_show_uri(NULL, "https://github.com/ehliar/schematic_gui/blob/master/tutorial/tutorial.md", GDK_CURRENT_TIME, NULL)){
		fprintf(stderr,"Error: Couldn't launch webbrowser to http://github.com/ehliar/schematic_gui/tutorial/tutorial.md   (perhaps GTK is not configured to show URLs?)\n");
		
	}
	return FALSE;
}

schematic_gui::schematic_gui(schematic_design *design, const char *filename)
{
	
        cairo_scale_factor = 1;
        cairo_translation_x = 0;
        cairo_translation_y = 0;
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        // FIXME: How to handle path to the glade file?
	GtkBuilder *build = gtk_builder_new_from_file(PREFIX "/share/schematic_gui/schematic_gui.glade");
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_save_as", G_CALLBACK(do_save_as));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_save", G_CALLBACK(do_save));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_new", G_CALLBACK(do_new));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_open", G_CALLBACK(do_open));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_recover", G_CALLBACK(do_recover));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_about", G_CALLBACK(do_about));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_export", G_CALLBACK(do_export));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_show_help", G_CALLBACK(do_show_help));
	gtk_builder_add_callback_symbol(build, "schematic_gui::do_show_tutorial", G_CALLBACK(do_show_tutorial));
	gtk_builder_connect_signals(build, this);
	GObject *menubar = gtk_builder_get_object(build, "menubar1");
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);


	
        g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), this);
        
        da = gtk_drawing_area_new();
        gtk_widget_set_size_request (da, WINDOW_WIDTH, WINDOW_HEIGHT);
        g_signal_connect (da, "configure-event", G_CALLBACK(configure_event),  this);
        g_signal_connect (da, "draw", G_CALLBACK(draw_cb),  this);
        g_signal_connect (da, "key-press-event", G_CALLBACK (press_key), this);
        g_signal_connect (da, "scroll-event", G_CALLBACK (scroll_event), this);
        g_signal_connect (da, "button-press-event", G_CALLBACK(press_button),  this);
        g_signal_connect (da, "button-release-event", G_CALLBACK(release_button),  this);
        g_signal_connect (da, "motion-notify-event", G_CALLBACK(motion_event), this);
        gtk_widget_set_events(da, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK | GDK_SCROLL_MASK);
        

        create_help_widget();
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menubar), FALSE, TRUE, 2);
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_box_pack_start(GTK_BOX(hbox), da, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), helpwindow, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 2);
	//        gtk_container_add (GTK_CONTAINER (window), da);
        gtk_container_add (GTK_CONTAINER (window), vbox);
	//        gtk_widget_show(da);
        gtk_widget_show_all (window);

	if(filename){
		set_filename(std::string(filename));
		current_filename = filename;
	}else{
		current_filename = "";
	}
        
        gtk_widget_set_can_focus(da, TRUE);
        gtk_widget_grab_focus(da);
        
        thedesign = design;
        print_preview_enabled = false;
}
