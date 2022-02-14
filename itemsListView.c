/**
 * File              : itemsListView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.02.2022
 * Last Modified Date: 12.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "itemsListView.h"
#include <stdio.h>

GtkTreeStore *itemsListViewStore;
int DATABASE;

enum {
  COLUMN_TITLE,
  COLUMN_UNIT,
  COLUMN_PRICE,
  COLUMN_ID,
  ITEM_POINTER,
  N_COLUMNS
};

GtkTreeStore *gstroybat_items_list_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS, 
			G_TYPE_STRING, //title 
			G_TYPE_STRING, //unit
			G_TYPE_INT, //price
			G_TYPE_INT, //ID
			G_TYPE_POINTER
	);

	return store;
}

void gstroybat_add_item_to_tree_store(GtkTreeStore *store, StroybatItem *item, GtkTreeIter *iter, GtkTreeIter *parent){
	gtk_tree_store_append(store, iter, parent);
	gtk_tree_store_set(store, iter, 
			COLUMN_TITLE, item->title, 
			COLUMN_UNIT, item->unit, 
			COLUMN_PRICE, item->price, 
			COLUMN_ID, item->id, 
			ITEM_POINTER, item, 
	-1);

}

int gtroybat_fill_items_list_with_items(StroybatItem *item, void *data, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}

	GtkTreeIter *parent = data;

	GtkTreeIter iter;
	gstroybat_add_item_to_tree_store(itemsListViewStore, item, &iter, parent);

	if (item->id > 0) {
		stroybat_get_all_items_from_database_for_parent(DATABASE, item->id, &iter, gtroybat_fill_items_list_with_items);
	}

	return 0;
}

void gstroybat_items_list_view_store_update(const char *search, GtkTreeStore *store, int DATABASE){
	gtk_tree_store_clear(store);
	if (search) {
		stroybat_get_all_items_from_database(DATABASE, search, NULL, gtroybat_fill_items_list_with_items);
		
	} else {
		stroybat_get_all_items_from_database_for_parent(DATABASE, 0, NULL, gtroybat_fill_items_list_with_items);
	}
}

void gstroybat_items_list_tree_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatItem *item;
		gtk_tree_model_get(model, &iter, ITEM_POINTER, &item, -1); 
		if (item) {
			if (item->id > 0) {
				gtk_tree_view_expand_row(treeview, path, false);
			} else {
				StroybatSmeta *smeta = g_object_get_data(G_OBJECT(treeview), "StroybatSmeta");

				StroybatItem *newItem = stroybat_item_new(NULL, item->title, item->unit, item->price, 1, DATABASE);

				stroybat_smeta_add_item(smeta->uuid, newItem);
				gstroybat_items_table_model_update(smeta);
			}

		} else {
			g_print("No Item data in row\n");
		}
	}
}

void gstroybat_smeta_items_list_changed(GtkWidget *widget, gpointer user_data){
	GtkTreeStore *store = user_data;

	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
	const char *search = gtk_entry_buffer_get_text(buffer);
	g_print("Search has changed to: %s\n", search);

	if (strlen(search) > 2) {
		gstroybat_items_list_view_store_update(search, store, DATABASE);	
	}

	if (strlen(search) == 0) {
		search = NULL;
		gstroybat_items_list_view_store_update(search, store, DATABASE);	
	}
}


void gstroybat_items_list_new(StroybatSmeta *smeta, GtkListStore *store, int _DATABASE){
	DATABASE = _DATABASE;

	itemsListViewStore = gstroybat_items_list_table_model_new();

	GtkWidget *win = gtk_window_new();
	gtk_window_set_modal(GTK_WINDOW(win), true);

	char *title;
	if (DATABASE == 0) {
		title = "Список услуг";	
	}
	if (DATABASE == -1) {
		title = "Список материалов";	
	}	

	gtk_window_set_title(GTK_WINDOW(win), title);

	GtkWidget *winbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_window_set_child(GTK_WINDOW(win), winbox);

	GtkWidget *search = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(search), "Поиск: название");
	g_signal_connect (search, "changed", G_CALLBACK (gstroybat_smeta_items_list_changed), itemsListViewStore);
	g_signal_connect (search, "insert-at-cursor", G_CALLBACK (gstroybat_smeta_items_list_changed), itemsListViewStore);	
	gtk_box_append(GTK_BOX(winbox), search);	
	gtk_widget_set_hexpand(search, TRUE);	

	GtkWidget *scrolledWindow = gtk_scrolled_window_new();
	gtk_widget_set_size_request(scrolledWindow, 600, 400);	
	gtk_widget_set_vexpand(scrolledWindow, TRUE);
	gtk_widget_set_hexpand(scrolledWindow, TRUE);
	gtk_box_append(GTK_BOX(winbox), scrolledWindow);	

	GtkWidget *treeView = gtk_tree_view_new();
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(itemsListViewStore));
	g_signal_connect(treeView, "row-activated", (GCallback) gstroybat_items_list_tree_view_row_activated, store);
	g_object_set_data(G_OBJECT(treeView), "itemsViewStore", store);
	g_object_set_data(G_OBJECT(treeView), "StroybatSmeta", smeta);
	g_object_set_data(G_OBJECT(treeView), "DATABASE", GINT_TO_POINTER(_DATABASE));

	const char *column_titles[] = {"Наименование", "Ед. изм.", "Цена"};

	int i;
	for (i = 0; i < 3; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 300, NULL);	
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i,  NULL);
		switch (i) {
			case COLUMN_TITLE:
				{
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
					g_object_set(column, "expand", TRUE, NULL);	
					break;
				}			
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
	}

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), treeView);

	gstroybat_items_list_view_store_update(NULL, itemsListViewStore, DATABASE);

	gtk_widget_show(win);
}
