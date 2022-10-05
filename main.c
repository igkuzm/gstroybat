/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 04.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include "chworkdir.h"
#include "openfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	GtkApplication *app = gtk_application_new ("kuzm.ig.gstroybat", 0);
	g_signal_connect (app, "activate", G_CALLBACK (gstroybat_application_on_activate), NULL); 

	//add Yandex Disk connection window (background) 
	YDConnect(app, YD_callback);
	
	//run gtk
	return g_application_run (G_APPLICATION (app), argc, argv);
}

void make_excel(){
	g_print("Make excel document\n");
	if (selectedSmeta) {
		GFile *template = g_file_new_for_path("Template.xlsx"); 
		GFile *file = g_file_new_for_path("tmp.xlsx"); 
		g_file_copy(template, file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
		
		stroybat_smeta_create_xlsx(database, selectedSmeta, "tmp.xlsx");
		openfile("tmp.xlsx");
	} else {
		g_print("Error to get smeta data!\n");
	}	
}
