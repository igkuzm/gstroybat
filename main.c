/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 01.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include "chworkdir.h"
#include <stdio.h>
#include <stdlib.h>

int init_database_callback(void *user_data, pthread_t threadid, char *msg){
	if (msg)
		g_print("%s\n", msg);

	return 0;
}

int main(int argc, char *argv[])
{
	//chworkdir
	k_lib_chWorkDir(argv);	

	//init database
	stroybat_init(database, "", NULL, init_database_callback);

	//init GTK
	GtkApplication *app = gtk_application_new ("kuzm.ig.gstroybat", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (gstroybat_application_on_activate), NULL); 

	return g_application_run (G_APPLICATION (app), argc, argv);
}
