/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "toast.h"
#include "getbundle.h"
#include "gstroybat.h"
#include "openfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int YD_callback(void *user_data, char *token, time_t expires, char *reftoken, char *error){
	GObject *app = user_data;
	if (token){
		g_print("TOKEN: %s", token);
		
		save_token_to_config(token, expires, reftoken);
		GtkToast * toast = g_object_get_data(app, "mainWindow_toast");
		gtk_toast_show_message(toast, "Yandex Disk подключен", 3, NULL, NULL, NULL, NULL);		

		/*needToUpdate = true;*/
		/*stroybat_init(DATABASE, token, NULL, init_database_callback);*/
	}
	return 0;
}

int main(int argc, char *argv[])
{
	//crete directory in home dir
	char workdir[BUFSIZ];
	sprintf(workdir, "%s/%s", g_get_home_dir(), "gstroybat");
	g_mkdir_with_parents(workdir, 0755);
	
	//copy files from bundle
	char *bundle = getbundle(argv);
	if (!bundle){
		printf("can't get application bundle\n");
		return 1;
	}
	char *files[] = {"stroybat.db", "Template.xlsx", "gstroybat.png", "License.md", NULL};
	for (int i = 0; files[i]; ++i) {
		GFile *sfile = g_file_new_build_filename(bundle,  files[i], NULL);
		GFile *dfile = g_file_new_build_filename(workdir, files[i], NULL);
		
		GError *error = NULL;
		g_file_copy(sfile, dfile, 0, NULL, NULL, NULL, &error);
		if (error)
			printf("g_file_copy error: %s\n", error->message);
	}

	//free bundle var
	free(bundle);

	//chworkdir
	chdir(workdir);	

	//get token
	char * token = token_from_config();
	printf("init with token: %s\n", token);

	//init database
	stroybat_init(DATABASE, token, NULL, init_database_callback);

	//init GTK
	GtkApplication *app = gtk_application_new ("kuzm.ig.gstroybat", 0);
	g_signal_connect (app, "activate", G_CALLBACK (gstroybat_application_on_activate), NULL); 

	//add Yandex Disk connection window (background) 
	YDConnect(app, YD_callback);
	
	//run gtk
	return g_application_run (G_APPLICATION (app), argc, argv);
}

void make_excel(GtkButton *button, gpointer userdata){
	g_print("Make excel document\n");
	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");	
	if (smeta) {
		GtkToast * toast = g_object_get_data(app, "mainWindow_toast");
		gtk_toast_show_message(toast, STR("Открываем EXCEL таблицу для %s", smeta->title), 3, NULL, NULL, NULL, NULL);

		GFile *template = g_file_new_for_path("Template.xlsx"); 
		GFile *file = g_file_new_for_path("tmp.xlsx"); 
		g_file_copy(template, file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);
		
		stroybat_smeta_create_xlsx(DATABASE, smeta, "tmp.xlsx");
		openfile("tmp.xlsx");
	} else {
		g_print("Error to get smeta data!\n");
	}	
}
