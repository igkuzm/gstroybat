/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 02.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include "chworkdir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	//chworkdir
	k_lib_chWorkDir(argv);	

	//get token
	char token[64] = {0};
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, "stroybat.ini", 0, NULL)){
		gchar * _token = g_key_file_get_value(key_file, "stroybat", "YDToken", NULL);
		strncpy(token, _token, 63);
		token[63] = 0;
	}

	printf("init with token: %s\n", token);

	//init database
	stroybat_init(database, token, NULL, init_database_callback);

	//init GTK
	GtkApplication *app = gtk_application_new ("kuzm.ig.gstroybat", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (gstroybat_application_on_activate), NULL); 

	return g_application_run (G_APPLICATION (app), argc, argv);
}
