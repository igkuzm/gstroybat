/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 10.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include <gtk/gtk.h>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
	GtkApplication *app = gtk_application_new ("kuzm.ig.gstroybat", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (gstroybat_application_on_activate), NULL); 

	return g_application_run (G_APPLICATION (app), argc, argv);
}
