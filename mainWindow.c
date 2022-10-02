/**
 * File              : mainWindow.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 02.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"


int YD_callback(void *user_data, char *token, time_t expires, char *reftoken, char *error){
	GtkApplication *app = user_data;
	if (token){
		g_print("TOKEN: %s", token);
		
		GKeyFile *key_file = g_key_file_new ();
		g_key_file_set_value(key_file, "stroybat", "YDToken", token);
		g_key_file_save_to_file(key_file, "stroybat.ini", NULL);

		GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Yandex Disk подключен");
		gtk_widget_show(dialog);
		
		/*needToUpdate = true;*/
		/*stroybat_init(database, token, NULL, init_database_callback);*/
	}
	return 0;
}

void gstroybat_application_on_activate (GtkApplication *app, gpointer user_data) {

	gstroybat_application_menu(app);
	
	GtkWidget *window = gtk_application_window_new(app);
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (window), TRUE);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gstroybat_application_on_deactivate), app); //quit application on window destroy

	GtkWidget *splitter = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_window_set_child(GTK_WINDOW(window), splitter);
	/*gtk_container_add(GTK_CONTAINER(window), splitter);*/
	
	/*GtkWidget *frame1 = gtk_frame_new (NULL);*/
	/*GtkWidget *frame2 = gtk_frame_new (NULL);*/
	/*gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_IN);*/
	/*gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_IN);*/
	/*gtk_container_add(GTK_CONTAINER(frame1), gstroybat_smeta_table_view_new(window));*/
	/*gtk_container_add(GTK_CONTAINER(frame2), gstroybat_items_table_view_new(window));*/
	
	gtk_paned_set_start_child(GTK_PANED(splitter), gstroybat_smeta_table_view_new(window));
	/*gtk_paned_pack1(GTK_PANED(splitter), frame1, TRUE, TRUE);*/
	gtk_paned_set_end_child(GTK_PANED(splitter), gstroybat_items_table_view_new(window));	
	/*gtk_paned_pack2(GTK_PANED(splitter), frame2, TRUE, TRUE);*/

	gtk_window_set_title(GTK_WINDOW(window), "Список смет");
	
	gtk_window_present (GTK_WINDOW (window));
	/*gtk_widget_show_all (GTK_WIDGET (window));*/

	YDConnect(app, YD_callback);
}

void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData) {
    g_application_quit (userData); // << and here
}

