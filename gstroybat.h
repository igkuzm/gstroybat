/**
 * File              : gstroybat.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GSTROYBAT_H
#define GSTROYBAT_H

#include <gtk/gtk.h>
#include <stdio.h>
#include "stroybat/libstroybat.h"
#include <stdbool.h>
#include "toast.h"
	
#define DATABASE "stroybat.db"

#ifdef __cplusplus
extern "C" {
#endif

#define STR(...) ({char str[BUFSIZ]; sprintf(str, __VA_ARGS__); str;})

//config
void save_window_state (GtkWidget *widget, GtkAllocation *allocation, gpointer userdata);
void window_save_state_to_config(const char * window_name, int x, int y, int width, int height);
void window_restore_state_from_config(GtkWindow * window, const char * window_name, int default_width, int default_height);
void save_token_to_config(const char * token, time_t expires, const char * reftoken);
char * token_from_config();

//selections	
static int totalPriceMaterials;
static int totalPriceServices;
static int totalPrice;

//update
static bool needToUpdate = false;
static int init_database_callback(void *user_data, pthread_t threadid, char *msg);

//application functions
void gstroybat_application_on_activate (GtkApplication *app, gpointer userData);
void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData);
void gstroybat_application_menu(GtkApplication *app);

//smeta view
GtkWidget *smeta_view_new(GObject * app);
void smeta_view_table_model_update(char * search);
void table_model_update(GObject * app, StroybatSmeta *smeta);
void store_add(GtkListStore *store, StroybatItem *item);

//smeta edit
void smeta_edit_new(StroybatSmeta *smeta);

//materials view
GtkWidget *materials_view_new(GObject * app);

//services view
GtkWidget *services_view_new(GObject * app);

//add items
void gstroybat_items_list_new(GObject * app, StroybatSmeta *smeta, GtkListStore *store, int datatype);

//Yandex Disk
void YDShow();
void YDConnect(
		void *user_data, 
		int (*callback)(
			void *user_data, 
			char *token, 
			time_t expires, 
			char *reftoken, 
			char *error
			)
		);

int init_database_callback(void *user_data, pthread_t threadid, char *msg){
	if (msg)
		g_print("%s\n", msg);

	if (needToUpdate)
		return 1; //stop execution

	return 0;
}

void make_excel(GtkButton *button, gpointer userdata);

void notification(GtkButton *button, gpointer userdata);

#ifdef __cplusplus
}
#endif

#endif //GSTROYBAT_H

