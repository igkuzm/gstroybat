/**
 * File              : gstroybat.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 12.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GSTROYBAT_H
#define GSTROYBAT_H

#include <gtk/gtk.h>
#include "stroybat/libstroybat.h"

GtkWidget *mainWindow;

void gstroybat_add_item_to_store(GtkListStore *store, StroybatItem *item);
void gstroybat_items_table_model_update(StroybatSmeta *smeta);

#endif //GSTROYBAT_H

