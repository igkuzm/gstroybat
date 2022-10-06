/**
 * File              : gstroybat.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 06.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GSTROYBAT_H
#define GSTROYBAT_H

#include <gtk/gtk.h>
#include <stdio.h>
#include "stroybat/libstroybat.h"
#include <stdbool.h>
	
#define DATABASE "stroybat.db"

#ifdef __cplusplus
extern "C" {
#endif

//windows
GtkWidget *mainWindow;

//views
GtkWidget *smetaView;

//selections	
StroybatSmeta * selectedSmeta;
StroybatItem  * selectedMaterial;
StroybatItem  * selectedService;
int totalPriceMaterials;
int totalPriceServices;
int totalPrice;

//buttons
GtkWidget * smetaAddButton;
GtkWidget * smetaRemoveButton;
GtkWidget * smetaEditButton;
GtkWidget * materialAddButton;
GtkWidget * materialRemoveButton;
GtkWidget * serviceAddButton;
GtkWidget * serviceRemoveButton;
GtkWidget * makeExcelButton;

//labels
GtkWidget * materialsLabel;
GtkWidget * servicesLabel;
GtkWidget * totalPriceLabel;

//serach
GtkWidget * smetaViewSearch;
char	  * smetaViewSearchString;


//update
static bool needToUpdate = false;
static int init_database_callback(void *user_data, pthread_t threadid, char *msg);

//application functions
void gstroybat_application_on_activate (GtkApplication *app, gpointer userData);
void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData);
void gstroybat_application_menu(GtkApplication *app);

//smeta view
GtkWidget *smeta_view_new();
void smeta_view_table_model_update();
void table_model_update(StroybatSmeta *smeta);
void store_add(GtkListStore *store, StroybatItem *item);

//smeta edit
void smeta_edit_new(StroybatSmeta *smeta);

//materials view
GtkWidget *materials_view_new();

//services view
GtkWidget *services_view_new();

//add items
void gstroybat_items_list_new(StroybatSmeta *smeta, GtkListStore *store, int datatype);

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

void make_excel();

#ifdef __cplusplus
}
#endif

#endif //GSTROYBAT_H

