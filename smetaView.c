/**
 * File              : smetaView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 11.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "smetaView.h"

enum {
  COLUMN_TITLE,
  COLUMN_DATE,
  COLUMN_TOTAL_PRICE,
  N_COLUMNS
};

GtkListStore *gstroybat_smeta_table_model_new(){
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	GDate *new_date = g_date_new();
	gtk_list_store_set(store, &iter, COLUMN_TITLE, "hello", COLUMN_DATE, new_date, COLUMN_TOTAL_PRICE, 100, -1);
	return store;
}

void cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
	guint i = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "my_column_num"));
	g_print("edit commited of column %d\n", i);
}

GtkWidget *gstroybat_smeta_table_view_new(){
	GtkListStore *store = gstroybat_smeta_table_model_new();

	GtkWidget *smetaTable = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "editable", TRUE, NULL);
	g_object_set_data(G_OBJECT(renderer), "my_column_num", GUINT_TO_POINTER(COLUMN_TITLE));
	g_signal_connect(renderer, "edited", (GCallback) cell_edited_callback, NULL);
	
	GtkTreeViewColumn *title = gtk_tree_view_column_new_with_attributes("Наименование", renderer, "text", COLUMN_TITLE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(smetaTable), title);

	GtkTreeViewColumn *date = gtk_tree_view_column_new_with_attributes("Дата", renderer, "text", COLUMN_DATE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(smetaTable), date);	

	GtkTreeViewColumn *price = gtk_tree_view_column_new_with_attributes("Сумма", renderer, "text", COLUMN_TOTAL_PRICE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(smetaTable), price);	

	return smetaTable;
}

