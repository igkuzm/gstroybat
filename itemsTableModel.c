/**
 * File              : itemsTableModel.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 03.10.2022
 * Last Modified Date: 03.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "itemsTableModel.h"

#define STR(...) ({char str[BUFSIZ]; sprintf(str, __VA_ARGS__); str;})

GtkListStore *table_model_new(){
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, 
			G_TYPE_STRING, //title
			G_TYPE_STRING, //unit
			G_TYPE_INT,    //price
			G_TYPE_INT,    //count
			G_TYPE_INT,    //total
			G_TYPE_POINTER
	);

	return store;
}

void store_add(GtkListStore *store, StroybatItem *item){
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

int fill_table(StroybatItem *item, void *data, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}

	GtkListStore *store;
	if (item->id == STROYBAT_MATERIALS){
		store = materialsViewStore;
		totalPriceMaterials += item->price * item->count;
	}
	if (item->id == STROYBAT_SERVICES){
		store = servicesViewStore;
		totalPriceServices += item->price * item->count;
	}	
	
	store_add(store, item);
	totalPrice += item->price * item->count;

	return 0;
}

gboolean table_model_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	StroybatItem *item;
	gtk_tree_model_get(model, iter, ITEM_POINTER, &item, -1);	
	free(item);
	return false;
}

void table_model_update(StroybatSmeta *smeta){
	gtk_tree_model_foreach (GTK_TREE_MODEL(materialsViewStore), table_model_free, NULL);
	gtk_list_store_clear(materialsViewStore);
	
	gtk_tree_model_foreach (GTK_TREE_MODEL(servicesViewStore), table_model_free, NULL);
	gtk_list_store_clear(servicesViewStore);

	totalPriceMaterials = 0;
	totalPriceServices = 0;
	totalPrice = 0;	

	stroybat_smeta_items_get(database, smeta->uuid, NULL, fill_table);

	gtk_label_set_text(GTK_LABEL(materialsLabel), STR("Материалы: %d руб.", totalPriceMaterials));	
	gtk_label_set_text(GTK_LABEL(servicesLabel), STR("Работы: %d руб.", totalPriceServices));	
	gtk_label_set_text(GTK_LABEL(totalPriceLabel), STR("Итого: %d руб.", totalPrice));	
}

void table_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	STROYBAT_DATA_TYPE datatype = GPOINTER_TO_INT(userdata);
	printf("DATATYPE: %d\n", datatype);

	if (datatype == STROYBAT_SERVICES){
		selectedService = NULL;
		gtk_widget_set_sensitive(serviceRemoveButton, false);
	}
	if (datatype == STROYBAT_MATERIALS){
		selectedMaterial = NULL;
		gtk_widget_set_sensitive(materialRemoveButton, false);
	}				

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatItem *item;
		gtk_tree_model_get(model, &iter, ITEM_POINTER, &item, -1); 			
		
		if (item){
			if (datatype == STROYBAT_SERVICES){
				selectedService = item;
				gtk_widget_set_sensitive(serviceRemoveButton, true);
			}
			if (datatype == STROYBAT_MATERIALS){
				selectedMaterial = item;
				gtk_widget_set_sensitive(materialRemoveButton, true);
			}			
		}
	}
}

void table_cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
	guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_number"));
	g_print("EDITED path: %s, col: %d\n", path_string, column_number);

	// we HAVE TO use GtkTreeView within gpointer!
	//  otherwise we could not differntiate the model type!
	GtkTreeView  *treeview = GTK_TREE_VIEW(user_data);
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
				int err = stroybat_item_set_value_for_key(database, item->uuid, new_text, "title");
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
				int err = stroybat_item_set_value_for_key(database, item->uuid, new_text, "unit");
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
				int err = stroybat_item_set_value_for_key(database, item->uuid, new_text, "unit");
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
				int err = stroybat_item_set_value_for_key(database, item->uuid, new_text, "count");
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

void ask_to_remove_item_responce(GtkDialog *dialog, gint arg1, gpointer user_data){
	if (arg1 == 1) {
		g_print("Remove commited\n");

		StroybatItem * item = user_data;
		if (!item){
			g_print("Item is NULL\n");
			gtk_window_destroy(GTK_WINDOW(dialog));
			return;
		}		
		
		if (stroybat_smeta_remove_item(database, item->uuid)){
			g_print("Error to remove Item!\n");
			gtk_window_destroy(GTK_WINDOW(dialog));
			return;
		}
			
		table_model_update(selectedSmeta);
	}
	gtk_window_destroy(GTK_WINDOW(dialog));
}

void ask_to_remove_item(StroybatItem * item) {
	if (!item){
		g_print("Item is NULL\n");
		return;
	}
	char *title;
	if (item->id == STROYBAT_SERVICES)
		title = STR("Удалить работу %s?", item->title);
	if (item->id == STROYBAT_MATERIALS)
		title = STR("Удалить материал %s?", item->title);
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"%s", title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");
	gtk_dialog_add_button(GTK_DIALOG(dialog), "УДАЛИТЬ", 1);
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", 0);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);
	g_signal_connect (dialog, "response", G_CALLBACK (ask_to_remove_item_responce), item);
	gtk_widget_show(dialog);
}

GtkWidget *items_view_new(GtkWidget *header, GtkListStore *store, STROYBAT_DATA_TYPE datatype){
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	
	//add header	
	gtk_box_append(GTK_BOX(box), header);	

	//add scrolled window
	GtkWidget *window = gtk_scrolled_window_new();
	gtk_widget_set_size_request (GTK_WIDGET(window), 900, 200);	
	gtk_widget_set_vexpand(window, TRUE);
	gtk_box_append(GTK_BOX(box), window);

	//create view
	GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), true);
	g_signal_connect(view, "row-activated", (GCallback) table_row_activated, GUINT_TO_POINTER(datatype));
	
	const char *column_titles[] = {"Наименование", "Ед. изм.", "Цена", "Количество", "Сумма"};
	int i;
	for (i = 0; i < N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
		g_signal_connect(renderer, "edited", (GCallback) table_cell_edited_callback, view);
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
