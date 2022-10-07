/**
 * File              : itemsTableModel.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 03.10.2022
 * Last Modified Date: 07.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef ITEMS_TABLE_MODEL_H
#define ITEMS_TABLE_MODEL_H

#include "gstroybat.h"

enum {
  COLUMN_TITLE,
  COLUMN_UNIT,
  COLUMN_PRICE,
  COLUMN_COUNT,
  COLUMN_TOTAL,
  ITEM_POINTER,
  N_COLUMNS
};

GtkListStore *table_model_new();

int fill_table(StroybatItem *item, void *data, char *error);

gboolean table_model_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data);

void table_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata);

void table_cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data);


void ask_to_remove_item_responce(GtkDialog *dialog, gint arg1, gpointer user_data);
void ask_to_remove_item();

GtkWidget *items_view_new(GtkWidget *header, GObject *app, GtkListStore *store, STROYBAT_DATA_TYPE datatype);

#endif /* ifndef ITEMS_TABLE_MODEL_H */
