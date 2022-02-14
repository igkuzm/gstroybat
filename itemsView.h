/**
 * File              : itemsView.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 12.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"

GtkWidget *addServiceButton;
GtkWidget *addProductButton;

GtkWidget *gstroybat_items_table_view_new(StroybatSmeta *smeta);

void gstroybat_items_table_model_update(StroybatSmeta *smeta);

