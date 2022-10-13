/**
 * File              : itemsTableModel.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 03.10.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "itemsTableModel.h"

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

int fill_table(StroybatItem *item, void *userdata, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}

	GtkListStore *store;
	GObject *app = userdata;
	if (item->id == STROYBAT_MATERIALS){
		store = g_object_get_data(app, "materialsViewStore");	
		totalPriceMaterials += item->price * item->count;
	}
	if (item->id == STROYBAT_SERVICES){
		store = g_object_get_data(app, "servicesViewStore");	
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

void table_model_update(GObject * app, StroybatSmeta *smeta){
	g_print("Update table model for smeta: %s\n", smeta->uuid);
	GtkListStore *materialsViewStore = g_object_get_data(app, "materialsViewStore");	
	GtkListStore *servicesViewStore  = g_object_get_data(app, "servicesViewStore");	
	
	gtk_tree_model_foreach (GTK_TREE_MODEL(materialsViewStore), table_model_free, NULL);
	gtk_list_store_clear(materialsViewStore);
	
	gtk_tree_model_foreach (GTK_TREE_MODEL(servicesViewStore), table_model_free, NULL);
	gtk_list_store_clear(servicesViewStore);

	totalPriceMaterials = 0;
	totalPriceServices = 0;
	totalPrice = 0;	

	stroybat_smeta_items_get(DATABASE, smeta->uuid, app, fill_table);

	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "materialsLabel")), STR("Материалы: %d руб.", totalPriceMaterials));	
	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "servicesLabel")), STR("Работы: %d руб.", totalPriceServices));	
	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "totalPriceLabel")), STR("Итого: %d руб.", totalPrice));	
}

void table_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");	

	STROYBAT_DATA_TYPE datatype = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(treeview), "datatype"));
	printf("DATATYPE: %d\n", datatype);

	if (datatype == STROYBAT_SERVICES){
		g_object_set_data(app, "selectedService", NULL);
		gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "serviceRemoveButton")), FALSE);		
	}
	if (datatype == STROYBAT_MATERIALS){
		g_object_set_data(app, "selectedMaterial", NULL);
		gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "materialRemoveButton")), FALSE);		
	}				

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatItem *item;
		gtk_tree_model_get(model, &iter, ITEM_POINTER, &item, -1); 			
		
		if (item){
			if (datatype == STROYBAT_SERVICES){
				g_object_set_data(app, "selectedService", item);
				gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "serviceRemoveButton")), TRUE);
			}
			if (datatype == STROYBAT_MATERIALS){
				g_object_set_data(app, "selectedMaterial", item);
				gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "materialRemoveButton")), TRUE);
			}			
		}
	}
}

void table_cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
	guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_number"));
	g_print("EDITED path: %s, col: %d\n", path_string, column_number);
	
	//get application delegate
	GObject *app = g_object_get_data(G_OBJECT(cell), "app");

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
				int err = stroybat_item_set_value_for_key(DATABASE, item->uuid, new_text, "title");
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
				int err = stroybat_item_set_value_for_key(DATABASE, item->uuid, new_text, "unit");
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
				int err = stroybat_item_set_value_for_key(DATABASE, item->uuid, new_text, "unit");
				if (err){
					g_print("Error to change item price! Err: %d\n", err);
				} else {
					//count new total price
					totalPrice -= item->price * item->count;
					totalPrice += price * item->count;
					gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "totalPriceLabel")), STR("Итого: %d руб.", totalPrice));	
					//count new material price
					if (item->id == STROYBAT_MATERIALS){
						totalPriceMaterials -= item->price * item->count;
						totalPriceMaterials += price * item->count;
						gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "materialsLabel")), STR("Материалы: %d руб.", totalPriceMaterials));	
					}
					//count new service price
					if (item->id == STROYBAT_SERVICES){
						totalPriceServices -= item->price * item->count;
						totalPriceServices += price * item->count;
						gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "servicesLabel")), STR("Работы: %d руб.", totalPriceServices));	
					}
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, price, COLUMN_TOTAL, price * item->count, -1);
					item->price = price;
				} 
				break;
			}			
		case COLUMN_COUNT:
			{
				g_print("Set Item Count to: %s\n", new_text);
				int count;
				sscanf(new_text, "%d", &count);
				int err = stroybat_item_set_value_for_key(DATABASE, item->uuid, new_text, "count");
				if (err){
					g_print("Error to change item price! Err: %d\n", err);
				} else {
					//count new total price
					totalPrice -= item->price * item->count;
					totalPrice += item->price * count;
					gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "totalPriceLabel")), STR("Итого: %d руб.", totalPrice));	
					//count new material price
					if (item->id == STROYBAT_MATERIALS){
						totalPriceMaterials -= item->price * item->count;
						totalPriceMaterials += item->price * count;
						gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "materialsLabel")), STR("Материалы: %d руб.", totalPriceMaterials));	
					}
					//count new service price
					if (item->id == STROYBAT_SERVICES){
						totalPriceServices -= item->price * item->count;
						totalPriceServices += item->price * count;
						gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "servicesLabel")), STR("Работы: %d руб.", totalPriceServices));	
					}					
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, count, COLUMN_TOTAL, count * item->price, -1);
					item->count = count;
				} 
				break;
			}			
		default: break;
	}
}

void on_item_remove_canceled(GtkToast *toast, void * cancel_data){
}

void on_item_removed(GtkToast *toast, void * userdata){
	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");		
	StroybatItem  * item  = g_object_get_data(app, "itemToRemove");

	if (stroybat_smeta_remove_item(DATABASE, item->uuid)){
		g_print("Error to remove Item!\n");
		return;
	}
		
	table_model_update(app, smeta);
}

void ask_to_remove_item_responce(GtkDialog *dialog, gint responce, gpointer userdata){
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		GObject *app = userdata;
		StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");		
		StroybatItem  * item  = g_object_get_data(app, "itemToRemove");

		if (!item){
			g_print("Item is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		
		
		GtkToast * toast = g_object_get_data(app, "mainWindow_toast");
		gtk_toast_show_message(toast, STR("Удаление %s", item->title), 3, NULL, on_item_remove_canceled, app, on_item_removed);	
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ask_to_remove_item(GObject *app, StroybatItem * item) {
	if (!item){
		g_print("Item is NULL\n");
		return;
	}
	g_object_set_data(app, "itemToRemove", item);
	char *title = "";
	if (item->id == STROYBAT_SERVICES)
		title = STR("Удалить работу: %s?", item->title);
	if (item->id == STROYBAT_MATERIALS)
		title = STR("Удалить материал: %s?", item->title);
	GtkWidget * mainWindow = g_object_get_data(app, "mainWindow"); 
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"%s", title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");

	//add remove button
	GtkWidget *button = gtk_button_new_with_label("УДАЛИТЬ");
	GtkStyleContext *context = gtk_widget_get_style_context(button);
	gtk_style_context_add_class(context, "destructive-action");
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button, GTK_RESPONSE_DELETE_EVENT);
	
	//add cancel button
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	g_signal_connect (dialog, "response", G_CALLBACK (ask_to_remove_item_responce), app);

	gtk_widget_show_all(dialog);
}

GtkWidget *items_view_new(GtkWidget *header, GObject *app, GtkListStore *store,  STROYBAT_DATA_TYPE datatype){
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	
	//add header	
	/*gtk_box_append(GTK_BOX(box), header);	*/
	gtk_container_add(GTK_CONTAINER(box), header);

	//add scrolled window
	/*GtkWidget *window = gtk_scrolled_window_new();*/
	GtkWidget *window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request (GTK_WIDGET(window), 900, 200);	
	gtk_widget_set_vexpand(window, TRUE);
	/*gtk_box_append(GTK_BOX(box), window);*/
	gtk_container_add(GTK_CONTAINER(box), window);

	//create view
	GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), true);
	g_object_set_data(G_OBJECT(view), "datatype", GUINT_TO_POINTER(datatype));
	g_signal_connect(view, "row-activated", (GCallback) table_row_activated, app);
	
	const char *column_titles[] = {"Наименование", "Ед. изм.", "Цена", "Количество", "Сумма"};
	int i;
	for (i = 0; i < N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
		g_signal_connect(renderer, "edited", (GCallback) table_cell_edited_callback, view);
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "app", app);
		
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

	/*gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(window), view);*/
	gtk_container_add(GTK_CONTAINER(window), view);

	return box;	
}
