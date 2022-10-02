/**
 * File              : gstroybat.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 02.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GSTROYBAT_H
#define GSTROYBAT_H

#include <gtk/gtk.h>
#include <stdio.h>
#include "stroybat/libstroybat.h"
#include <stdbool.h>

	
#define database "stroybat.db"

#ifdef __cplusplus
extern "C" {
#endif

static bool needToUpdate = false;
static int init_database_callback(void *user_data, pthread_t threadid, char *msg);

void gstroybat_application_on_activate (GtkApplication *app, gpointer userData);
void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData);
void gstroybat_application_menu(GtkApplication *app);

GtkWidget *gstroybat_smeta_table_view_new(GtkWidget* mainWindow);

GtkWidget *gstroybat_items_table_view_new(GtkWidget* mainWindow);
void gstroybat_items_table_model_update(StroybatSmeta *smeta);

void gstroybat_add_item_to_store(GtkListStore *store, StroybatItem *item);
void gstroybat_items_table_model_update(StroybatSmeta *smeta);
void gstroybat_items_list_new(StroybatSmeta *smeta, GtkListStore *store, int datatype);

void YDConnectInit();
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


#ifdef __cplusplus
}
#endif

#endif //GSTROYBAT_H

