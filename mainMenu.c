/**
 * File              : mainMenu.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.03.2022
 * Last Modified Date: 16.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include <stdio.h>

static void yd_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	YDShow();
}

static void excel_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GObject *app = user_data;
	GtkButton * makeExcelButton = g_object_get_data(app, "makeExcelButton"); 
	make_excel(makeExcelButton, user_data);
}

void about_responce(GtkDialog *dialog, gint responce, gpointer userdata){
	if (responce == GTK_RESPONSE_CLOSE) {
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}	
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void about_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GError *error;
	const char *text = "Помощь строительным организациям в составлении смет";
	const char *version = "1.0";
	const char *authors[] = {"Игорь Семенцов", "Алексей Николайчук", NULL}; 
	

	char *license; gsize size;	
	error = NULL;
	g_file_get_contents("License.md", &license, &size, &error);
	if (error)
		g_error("%s", error->message);

	error = NULL;
	GdkPixbuf *icon = gdk_pixbuf_new_from_file("icon.png", &error);
	if (error)
		g_error("%s", error->message);	

	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), icon);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "GStroyBat");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), version);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "ООО \"Айти Пром\"");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), text);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_BSD_3);
	if(license){
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog), license);
		free(license);
	}
	
	g_signal_connect (dialog, "response", G_CALLBACK (about_responce), NULL);

	gtk_widget_show(dialog);
}


static void preferences_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{

}

static void quit_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	g_print("QUIT\n");
	g_application_quit(G_APPLICATION(user_data));
}

const GActionEntry app_actions[] = {
	{ "quit", quit_cb },
	{ "excel", excel_cb },
	{ "yd", yd_cb },
	{ "preferences", preferences_cb },
	{ "about", about_cb }
};

void gstroybat_application_menu(GtkApplication *app){
	g_action_map_add_action_entries (
			G_ACTION_MAP (app), 
			app_actions, 
			G_N_ELEMENTS (app_actions), 
			app); //activate menu actions

	//app menu for MacOS
#ifdef __APPLE__
	GMenu *appmenu = g_menu_new(); //main application menu
	
	GMenu *app_s1 = g_menu_new(); //section 1
	g_menu_append (app_s1, "О приложении", "app.about");  
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s1));
	g_object_unref (app_s1);
	
	GMenu *app_s2 = g_menu_new(); //section 2
	g_menu_append (app_s2, "Настройки", "app.preferences");
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s2));
	static const gchar *prefs[] = {"<meta>comma", NULL}; 
	gtk_application_set_accels_for_action(app, "app.preferences", prefs);	
	g_object_unref (app_s2);
	
	GMenu *app_s3 = g_menu_new(); //section 3
	g_menu_append (app_s3, "Завершить «GStroyBat»", "app.quit");  
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s3));
	static const gchar *quit[] = {"<meta>q", NULL}; 
	gtk_application_set_accels_for_action(app, "app.quit", quit);	
	g_object_unref (app_s3);
	
	gtk_application_set_app_menu(GTK_APPLICATION (app), G_MENU_MODEL(appmenu));
	g_object_unref (appmenu);
#endif

	//MENUBAR - for all systems
	GMenu *menuBar = g_menu_new (); //main menu bar
	
	//file menu
	GMenu *filemenu = g_menu_new(); //file item
	g_menu_append_submenu(menuBar, "Файл", G_MENU_MODEL(filemenu)); 

	//make excel
	g_menu_append (filemenu, "Вывод в EXCEL", "app.excel"); //open excel doc 
#ifdef __APPLE__
	static const gchar *excel[] = {"<meta>e", NULL}; 
#else	
	static const gchar *excel[] = {"<control>e", NULL}; 
#endif
	gtk_application_set_accels_for_action(app, "app.excel", excel);	

	//yandex disk
	g_menu_append (filemenu, "Yandex Disk", "app.yd"); //yandex disk
	gtk_application_set_accels_for_action(app, "app.yd", NULL);	 

	//Edit menu
	GMenu *edit = g_menu_new(); //edit item
	g_menu_append_submenu(menuBar, "Правка", G_MENU_MODEL(edit)); 
	
	g_menu_append (edit, "Копировать", "app.copy");  
	g_menu_append (edit, "Вставить", "app.paste");  

#ifndef __APPLE__
	//Help menu
	GMenu *help = g_menu_new(); //edit item
	g_menu_append_submenu(menuBar, "Помощь", G_MENU_MODEL(help)); 

	//add to menubar for not apple
	//preferences
	GMenu *preferences_section = g_menu_new(); //section
	g_menu_append (preferences_section, "Настройки", "app.preferences");	
	static const gchar *prefs[] = {"<control>comma", NULL}; 
	gtk_application_set_accels_for_action(app, "app.preferences", prefs);	
	g_menu_append_section (filemenu, NULL, G_MENU_MODEL(preferences_section));
	//quit
	GMenu *quit_section = g_menu_new(); //section
	g_menu_append(quit_section, "Завершить «GStroyBat»", "app.quit"); 
	static const gchar *quit[] = {"<control>q", NULL}; 
	gtk_application_set_accels_for_action(app, "app.quit", quit);	
	g_menu_append_section (filemenu, NULL, G_MENU_MODEL(quit_section));
	//help
	GMenu *about_section = g_menu_new(); //section
	g_menu_append (about_section, "О приложении", "app.about");  
	g_menu_append_section (help, NULL, G_MENU_MODEL(about_section));
	g_object_unref (help);
#endif	
	gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menuBar));
	g_object_unref (filemenu);
	g_object_unref (edit);
	g_object_unref (menuBar);	
}
