/**
 * File              : gstroybat.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 15.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GSTROYBAT_H
#define GSTROYBAT_H

#include <gtk/gtk.h>
#include "stroybat/libstroybat.h"

void gstroybat_application_on_activate (GtkApplication *app, gpointer userData);
void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData);
void gstroybat_application_menu(GtkApplication *app);

GtkWidget *gstroybat_smeta_table_view_new(GtkWidget* mainWindow);

GtkWidget *gstroybat_items_table_view_new(GtkWidget* mainWindow);
void gstroybat_items_table_model_update(StroybatSmeta *smeta);

void gstroybat_add_item_to_store(GtkListStore *store, StroybatItem *item);
void gstroybat_items_table_model_update(StroybatSmeta *smeta);
void gstroybat_items_list_new(StroybatSmeta *smeta, GtkListStore *store, int DATABASE);

#endif //GSTROYBAT_H

