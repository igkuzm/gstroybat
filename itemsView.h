/**
 * File              : itemsView.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef ITEMSVIEW_H
#define ITEMSVIEW_H

#include "gstroybat.h"


GtkWidget *gstroybat_items_table_view_new(GtkWidget* mainWindow);

void gstroybat_items_table_model_update(StroybatSmeta *smeta);

#endif
