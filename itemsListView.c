/**
 * File              : itemsListView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.02.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include <stdio.h>
#include <stdbool.h>

static GtkTreeStore *itemsListViewStore;
GtkWidget *treeView;
int datatype;

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
		stroybat_data_get_for_parent(DATABASE, datatype, item->id, &iter, gtroybat_fill_items_list_with_items);
	}

	return 0;
}

gboolean gstroybat_items_list_view_store_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	StroybatItem *item;
	gtk_tree_model_get(model, iter, ITEM_POINTER, &item, -1);	
	free(item);
	return false;
}

void gstroybat_items_list_view_store_update(const char *search, GtkTreeStore *store, int datatype){
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), gstroybat_items_list_view_store_free, NULL);
	gtk_tree_store_clear(store);
	
	if (search) {
		stroybat_data_get(DATABASE, datatype, search, NULL, gtroybat_fill_items_list_with_items);
		
	} else {
		stroybat_data_get_for_parent(DATABASE, datatype, 0, NULL, gtroybat_fill_items_list_with_items);
	}
}

void gstroybat_items_list_tree_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");
	GObject *app = userdata;	
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");	

	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatItem *item;
		gtk_tree_model_get(model, &iter, ITEM_POINTER, &item, -1); 
		if (item) {
			if (item->id > 0) {
				gtk_tree_view_expand_row(treeview, path, FALSE);
			} else {
				GtkToast * toast = g_object_get_data(app, "itemsList_toast");
				gtk_toast_show_message(toast, STR("Добавлено: %s в смету: %s", item->title, smeta->title), 3, NULL, NULL, NULL, NULL);				
				g_print("Add item: %s for smeta: %s", item->uuid, smeta->uuid);
				StroybatItem *newItem = stroybat_item_new(NULL, item->title, item->unit, item->price, 1, datatype);
				stroybat_smeta_add_item(DATABASE, smeta->uuid, newItem);
				table_model_update(app, smeta);
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
		gstroybat_items_list_view_store_update(search, store, datatype);	
	}

	if (strlen(search) == 0) {
		search = NULL;
		gstroybat_items_list_view_store_update(search, store, datatype);	
	}
}


void gstroybat_items_list_new(GObject * app, StroybatSmeta *smeta, GtkListStore *store, int _datatype){
	g_print("Select items for smeta: %s", smeta->title);
	datatype = _datatype;

	itemsListViewStore = gstroybat_items_list_table_model_new();

	//GtkWidget *win = gtk_window_new();
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_modal(GTK_WINDOW(win), TRUE);
	window_restore_state_from_config(GTK_WINDOW(win), "itemsList", 600, 400);
	g_signal_connect(G_OBJECT(win), "size-allocate", G_CALLBACK(save_window_state), "itemsList"); //save window state

	char *title;
	if (datatype == 0) {
		title = "Список работ";	
	}
	if (datatype == -1) {
		title = "Список материалов";	
	}	

	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

	//window overlay
	GtkWidget *overlay = gtk_overlay_new(); 
	gtk_container_add (GTK_CONTAINER (win), overlay);	

	GtkWidget *winbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	//gtk_window_set_child(GTK_WINDOW(win), winbox);
	gtk_container_add(GTK_CONTAINER(overlay), winbox);
	//gtk_container_add(GTK_CONTAINER(win), winbox);

	GtkWidget *search = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(search), "Поиск: название");
	g_signal_connect (search, "changed", G_CALLBACK (gstroybat_smeta_items_list_changed), itemsListViewStore);
	g_signal_connect (search, "insert-at-cursor", G_CALLBACK (gstroybat_smeta_items_list_changed), itemsListViewStore);	

	//gtk_box_append(GTK_BOX(winbox), search);	
	gtk_container_add(GTK_CONTAINER(winbox), search);
	gtk_widget_set_hexpand(search, TRUE);	

	//GtkWidget *scrolledWindow = gtk_scrolled_window_new();
	GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolledWindow, 600, 400);	
	gtk_widget_set_vexpand(scrolledWindow, TRUE);
	gtk_widget_set_hexpand(scrolledWindow, TRUE);
	//gtk_box_append(GTK_BOX(winbox), scrolledWindow);	
	gtk_container_add(GTK_CONTAINER(winbox), scrolledWindow);

	treeView = gtk_tree_view_new();
	//gtk_tree_view_set_search_entry(GTK_TREE_VIEW(treeView), search);
	//gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeView), true);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(itemsListViewStore));
	g_signal_connect(treeView, "row-activated", (GCallback) gstroybat_items_list_tree_view_row_activated, app);
	g_object_set_data(G_OBJECT(treeView), "itemsViewStore", store);
	g_object_set_data(G_OBJECT(treeView), "StroybatSmeta", smeta);

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

	//gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), treeView);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), treeView);

	gstroybat_items_list_view_store_update(NULL, itemsListViewStore, datatype);

	//add notification toast
	GtkWidget * toast = gtk_toast_new(); 
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), toast);
	g_object_set_data(G_OBJECT(app), "itemsList_toast", toast);	

	//gtk_widget_show(win);
	gtk_widget_show_all(win);
}
