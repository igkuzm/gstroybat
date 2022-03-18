/**
 * File              : mainWindow.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 15.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"


void gstroybat_application_on_activate (GtkApplication *app, gpointer user_data) {

	gstroybat_application_menu(app);
	
	GtkWidget *window = gtk_application_window_new(app);
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (window), TRUE);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gstroybat_application_on_deactivate), app); //quit application on window destroy

	GtkWidget *splitter = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_window_set_child(GTK_WINDOW(window), splitter);
	
	gtk_paned_set_start_child(GTK_PANED(splitter), gstroybat_smeta_table_view_new(window));
	gtk_paned_set_end_child(GTK_PANED(splitter), gstroybat_items_table_view_new(window));	

	gtk_window_set_title(GTK_WINDOW(window), "Список смет");
	
	gtk_window_present (GTK_WINDOW (window));
}

void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData) {
    g_application_quit (userData); // << and here
}

