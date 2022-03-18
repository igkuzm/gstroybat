/**
 * File              : mainMenu.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.03.2022
 * Last Modified Date: 15.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"

static void gstroybat_app_menu_open_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	g_print("OPEN\n");
}

static void gstroybat_app_menu_quit_cb (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	g_print("QUIT\n");
	g_application_quit(G_APPLICATION(user_data));
}

const GActionEntry gstroybat_app_menu_app_actions[] = {
	{ "quit", gstroybat_app_menu_quit_cb },
	{ "open", gstroybat_app_menu_open_cb },
	{ "preferences", gstroybat_app_menu_open_cb },
	//{ "about", about_cb },
};

void gstroybat_application_menu(GtkApplication *app){
	g_action_map_add_action_entries (G_ACTION_MAP (app), gstroybat_app_menu_app_actions, G_N_ELEMENTS (gstroybat_app_menu_app_actions), app); //activate menu actions

	GMenu *menuBar = g_menu_new (); //main menu bar
	
	GMenu *appmenu = g_menu_new(); //main application menu
	g_menu_append_submenu(menuBar, "GStroyBat", G_MENU_MODEL(appmenu)); 
	
	GMenu *app_s1 = g_menu_new(); //section 1
	g_menu_append (app_s1, "О приложении", "app.about");  
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s1));
	g_object_unref (app_s1);
	
	GMenu *app_s2 = g_menu_new(); //section 2
	g_menu_append (app_s2, "Настройки", "app.preferences");
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s2));
#ifdef __APPLE__
	static const gchar *prefs[] = {"<meta>comma", NULL}; 
#else	
	static const gchar *prefs[] = {"<control>comma", NULL}; 
#endif
	gtk_application_set_accels_for_action(app, "app.preferences", prefs);	
	g_object_unref (app_s2);
	
	GMenu *app_s3 = g_menu_new(); //section 3
	g_menu_append (app_s3, "Завершить «GStroyBat»", "app.quit");  
	g_menu_append_section (appmenu, NULL, G_MENU_MODEL(app_s3));
#ifdef __APPLE__
	static const gchar *quit[] = {"<meta>q", NULL}; 
#else	
	static const gchar *quit[] = {"<control>q", NULL}; 
#endif
	gtk_application_set_accels_for_action(app, "app.quit", quit);	
	g_object_unref (app_s3);
	g_object_unref (appmenu);

	//MENUBAR
	GMenu *filemenu = g_menu_new(); //file item
	g_menu_append_submenu(menuBar, "Файл", G_MENU_MODEL(filemenu)); 
	g_menu_append (filemenu, "Открыть/Поиск", "app.open"); 
#ifdef __APPLE__
	static const gchar *open[] = {"<meta>o", NULL}; 
#else	
	static const gchar *open[] = {"<control>o", NULL}; 
#endif
	gtk_application_set_accels_for_action(app, "app.open", open);	
	g_object_unref (filemenu);

	GMenu *edit = g_menu_new(); //edit item
	g_menu_append_submenu(menuBar, "Правка", G_MENU_MODEL(edit)); 
	g_menu_append (edit, "Копировать", "app.copy");  
	g_menu_append (edit, "Вставить", "app.paste");  
	g_object_unref (edit);

	gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menuBar));
	g_object_unref (menuBar);	
}
