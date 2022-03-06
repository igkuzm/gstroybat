/**
 * File              : itemsView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 06.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "itemsView.h"
#include "itemsListView.h"
#include <stdio.h>
#include <stdlib.h>

GtkListStore *itemsViewStore;
GtkWidget *itemRemoveButton;
StroybatSmeta *selectedSmeta;

enum {
  COLUMN_TITLE,
  COLUMN_UNIT,
  COLUMN_PRICE,
  COLUMN_COUNT,
  COLUMN_TOTAL,
  ITEM_POINTER,
  N_COLUMNS
};

GtkListStore *gstroybat_items_table_model_new(){
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, 
			G_TYPE_STRING, //title 
			G_TYPE_STRING, //unit
			G_TYPE_INT, //price
			G_TYPE_INT, //count
			G_TYPE_INT, //total
			G_TYPE_POINTER
	);

	return store;
}

void gstroybat_add_item_to_store(GtkListStore *store, StroybatItem *item){
	
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			COLUMN_TITLE, item->title, 
			COLUMN_UNIT, item->unit, 
			COLUMN_PRICE, item->price, 
			COLUMN_COUNT, item->count, 
			COLUMN_TOTAL, item->count * item->price, 
			ITEM_POINTER, item, 
	-1);

}

int gtroybat_fill_table_with_items(StroybatItem *item, void *data, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}

	GtkListStore *store = data;
	gstroybat_add_item_to_store(store, item);

	return 0;
}

gboolean gstroybat_items_table_model_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	StroybatItem *item;
	gtk_tree_model_get(model, iter, ITEM_POINTER, &item, -1);	
	free(item);
	return false;
}

void gstroybat_items_table_model_update(StroybatSmeta *smeta){
	selectedSmeta = smeta;
	gtk_tree_model_foreach (GTK_TREE_MODEL(itemsViewStore), gstroybat_items_table_model_free, NULL);
	gtk_list_store_clear(itemsViewStore);

	stroybat_get_items_for_smeta(smeta->uuid, itemsViewStore, gtroybat_fill_table_with_items);
}

void gstroybat_items_table_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatItem *item;
		gtk_tree_model_get(model, &iter, ITEM_POINTER, &item, -1); 			
		gtk_widget_set_sensitive(itemRemoveButton, true);
		g_object_set_data(G_OBJECT(itemRemoveButton), "StroybatItem", item);
	} else {
		gtk_widget_set_sensitive(itemRemoveButton, false);
		g_object_set_data(G_OBJECT(itemRemoveButton), "StroybatItem", NULL);
	}	
}

void gstroybat_item_table_cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
	guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_number"));
	g_print("EDITED PATH: %s IN COLUMN: %d\n", path_string, column_number);

	// we HAVE TO use GtkTreeView within gpointer!
	//  otherwise we could not differntiate the model type!
	GtkTreeView *treeview = GTK_TREE_VIEW(user_data);
	GtkTreeModel *treeModel = gtk_tree_view_get_model(treeview);

	// we need to use GtkListStore, because GtkTreeModel does not
	//  provide an interface for cell changing.
	GtkListStore *model;   
	GtkTreeIter iter_rawModel;

	// check if we're working on the raw model or on a sorted version
	//  of it
	if(GTK_IS_LIST_STORE(treeModel)){
		// just use the model as is    
		model = GTK_LIST_STORE(treeModel);

		// retrieve the iterator for the cell, that should be changed
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)model, &iter_rawModel, path_string);

	} else { // we're working on a sorted model   
		// We need to change to a usual model.
		GtkTreeModelSort *sortedModel = GTK_TREE_MODEL_SORT(treeModel);
		model = GTK_LIST_STORE(gtk_tree_model_sort_get_model(sortedModel));

		// get the iterator within the sorted model
		GtkTreeIter iter_sortedModel;
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)sortedModel, &iter_sortedModel, path_string);  

		// convert the iterator to one of the raw model.
		// (Otherwise the wrong cell will change)
		gtk_tree_model_sort_convert_iter_to_child_iter(sortedModel, &iter_rawModel, &iter_sortedModel);
    }

	StroybatItem *item;
	gtk_tree_model_get(treeModel, &iter_rawModel, ITEM_POINTER, &item, -1); 	

	switch (column_number) {
		case COLUMN_TITLE:
			{
				g_print("Set Item Title to: %s\n", new_text);
				int err = stroybat_set_item_title(item->uuid, new_text);
				if (err){
					g_print("Error to change item title! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}
		case COLUMN_UNIT:
			{
				g_print("Set Item Unit to: %s\n", new_text);
				int err = stroybat_set_item_unit(item->uuid, new_text);
				if (err){
					g_print("Error to change item unit! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		case COLUMN_PRICE:
			{
				g_print("Set Item Price to: %s\n", new_text);
				int price;
				sscanf(new_text, "%d", &price);
				int err = stroybat_set_item_price(item->uuid, price);
				if (err){
					g_print("Error to change item price! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, price, COLUMN_TOTAL, price * item->count, -1);
				} 
				break;
			}			
		case COLUMN_COUNT:
			{
				g_print("Set Item Count to: %s\n", new_text);
				int count;
				sscanf(new_text, "%d", &count);
				int err = stroybat_set_item_count(item->uuid, count);
				if (err){
					g_print("Error to change item price! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, count, COLUMN_TOTAL, count * item->price, -1);
				} 
				break;
			}			
		default: break;
	}
}


void gstroybat_items_add_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Add button clicked\n");
	
	GtkListStore *store = user_data;
	StroybatSmeta *smeta = g_object_get_data(G_OBJECT(button), "StroybatSmeta");
	
	guint DATABASE = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "DATABASE"));

	if (store && smeta) {
		gstroybat_items_list_new(smeta, store, DATABASE);
	} else {
		g_print("Error to start Items List View!\n");
	}
}

void gstroybat_ask_to_remove_item_responce(GtkDialog *dialog, gint arg1, gpointer user_data){
	if (arg1 == 1) {
		g_print("Remove smeta button pushed\n");
		GtkListStore *store = user_data;
		StroybatItem *item = g_object_get_data(G_OBJECT(dialog), "StroybatItem");
		
		int err = stroybat_smeta_remove_item(selectedSmeta->uuid, item->uuid);
		if (err) {
			g_print("Error to remove Item! Err: %d\n", err);
		}
		else {
			gstroybat_items_table_model_update(selectedSmeta);
		}
	}
	gtk_window_destroy(GTK_WINDOW(dialog));
}

void gstroybat_ask_to_remove_item(GtkListStore *store, StroybatItem *item, GtkWidget *window) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"Удалить услугу/материалы %s?", item->title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");
	gtk_dialog_add_button(GTK_DIALOG(dialog), "УДАЛИТЬ", 1);
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", 0);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);
	g_object_set_data(G_OBJECT(dialog), "StroybatItem", item);
	g_signal_connect (dialog, "response", G_CALLBACK (gstroybat_ask_to_remove_item_responce), store);
	gtk_widget_show(dialog);
}

void gstroybat_item_remove_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Remove button clicked\n");
	
	GtkListStore *store = user_data;
	StroybatItem *item = g_object_get_data(G_OBJECT(button), "StroybatItem");
	
	if (item) {
		g_print("Remove Item: %s\n", item->title);
		GtkWidget* mainWindow = g_object_get_data(G_OBJECT(button), "mainWindow");
		gstroybat_ask_to_remove_item(store, item, mainWindow);
	} else {
		g_print("Error to get item data!\n");
	}
}

GtkWidget *gstroybat_items_table_view_header(GtkListStore *store, GtkWidget* mainWindow){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	GtkWidget *header_title = gtk_label_new("Услуги и материалы");
	gtk_widget_set_hexpand(header_title, TRUE);
	gtk_box_append(GTK_BOX(header), header_title);	

	GtkWidget* addServiceButton = gtk_button_new_with_label("+ услуга");
	g_signal_connect(addServiceButton, "clicked", (GCallback)gstroybat_items_add_button_pushed, store);
	g_object_set_data(G_OBJECT(addServiceButton), "DATABASE", GINT_TO_POINTER(0));
	g_object_set_data(G_OBJECT(mainWindow), "addServiceButton", addServiceButton);
	gtk_box_append(GTK_BOX(header), addServiceButton);	

	GtkWidget* addProductButton = gtk_button_new_with_label("+ товар");
	g_signal_connect(addProductButton, "clicked", (GCallback)gstroybat_items_add_button_pushed, store);
	g_object_set_data(G_OBJECT(addProductButton), "DATABASE", GINT_TO_POINTER(-1));
	g_object_set_data(G_OBJECT(mainWindow), "addProductButton", addProductButton);
	gtk_box_append(GTK_BOX(header), addProductButton);		
	
	itemRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(itemRemoveButton, false);
	g_object_set_data(G_OBJECT(itemRemoveButton), "mainWindow", mainWindow);
	
	g_signal_connect(itemRemoveButton, "clicked", (GCallback)gstroybat_item_remove_button_pushed, store);
	gtk_box_append(GTK_BOX(header), itemRemoveButton);	

	return header;
}

GtkWidget *gstroybat_items_table_view_new(GtkWidget* mainWindow){
	
	//create new Store
	itemsViewStore = gstroybat_items_table_model_new();
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	
	gtk_box_append(GTK_BOX(box), gstroybat_items_table_view_header(itemsViewStore, mainWindow));	

	GtkWidget *window = gtk_scrolled_window_new();
	gtk_widget_set_size_request (GTK_WIDGET(window), 900, 200);	
	gtk_widget_set_vexpand(window, TRUE);
	gtk_box_append(GTK_BOX(box), window);

	GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(itemsViewStore));
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), true);
	g_signal_connect(view, "row-activated", (GCallback) gstroybat_items_table_view_row_activated, itemsViewStore);
	

	const char *column_titles[] = {"Наименование", "Ед. изм.", "Цена", "Количество", "Сумма"};

	int i;
	for (i = 0; i < N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
		g_signal_connect(renderer, "edited", (GCallback) gstroybat_item_table_cell_edited_callback, view);
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i,  NULL);
		switch (i) {
			case COLUMN_TITLE:
				{
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
					g_object_set(column, "expand", TRUE, NULL);	
					g_object_set(renderer, "wrap-width", 300, NULL);	
					break;
				}			
			case COLUMN_TOTAL: 
				{
					renderer = gtk_cell_renderer_text_new(); 
					column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i,  NULL);
					break;
				};
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);	
	}

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(window), view);
	

	return box;

}

