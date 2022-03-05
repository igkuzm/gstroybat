/**
 * File              : smetaView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "smetaView.h"
#include "itemsView.h"
#include "openfile.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cp.h"

char *search;

GtkWidget *smetaTable;

enum {
  COLUMN_TITLE = 0,
  COLUMN_DATE,
  COLUMN_TOTAL_PRICE,
  COLUMN_ZAKAZCHIK,
  COLUMN_PODRIADCHIK,
  COLUMN_OBIEKT,
  COLUMN_RABOTI,
  COLUMN_OSNOVANIYE,
  SMETA_POINTER,
  N_COLUMNS
};

void gstroybat_add_smeta_to_store(GtkListStore *store, StroybatSmeta *smeta){
	
	struct tm *tm = localtime(&smeta->date);
	char date[11];
	//15.06.2009
	strftime(date, 32, "%d.%m.%Y", tm);  

	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
				COLUMN_TITLE, smeta->title, 
				COLUMN_DATE, date, 
				COLUMN_TOTAL_PRICE, stroybat_smeta_total_price(smeta->uuid), 
				COLUMN_ZAKAZCHIK, smeta->zakazchik, 
				COLUMN_PODRIADCHIK, smeta->podriadchik, 
				COLUMN_OBIEKT, smeta->obiekt, 
				COLUMN_RABOTI, smeta->raboti, 
				COLUMN_OSNOVANIYE, smeta->osnovaniye, 
				SMETA_POINTER, smeta, 
	-1);
}

int gtroybat_fill_table_with_smeta(StroybatSmeta *smeta, void *data, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}
	GtkListStore *store = data;
	gstroybat_add_smeta_to_store(store, smeta);

	return 0;
}

GtkListStore *gstroybat_smeta_table_model_new(){
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, 
			G_TYPE_STRING, //title 
			G_TYPE_STRING, //date
			G_TYPE_INT, //total price
			G_TYPE_STRING, //zakazchik
			G_TYPE_STRING, //podriadchik
			G_TYPE_STRING, //obiekt
			G_TYPE_STRING, //raboti
			G_TYPE_STRING, //osnovaniye
			G_TYPE_POINTER); //smeta pointer

	return store;
}

gboolean gstroybat_smeta_table_model_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	StroybatSmeta *smeta;
	gtk_tree_model_get(model, iter, SMETA_POINTER, &smeta, -1);	
	free(smeta);
	return false;
}

void gstroybat_smeta_table_model_update(GtkListStore *store, const char *search){
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), gstroybat_smeta_table_model_free, NULL);
	gtk_list_store_clear(store);
	stroybat_get_all_smeta(search, store, gtroybat_fill_table_with_smeta);
}

void gstroybat_smeta_table_cell_edited_callback (GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer user_data){
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

	StroybatSmeta *smeta;
	gtk_tree_model_get(treeModel, &iter_rawModel, SMETA_POINTER, &smeta, -1); 	

	switch (column_number) {
		case COLUMN_TITLE:
			{
				g_print("Set Smeta Title to: %s\n", new_text);
				int err = stroybat_set_smeta_title(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta title! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}
		case COLUMN_DATE:
			{
				g_print("Set Smeta Date to: %s\n", new_text);
				struct tm tm;
				int c = sscanf(new_text, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);	
				if (c != 3) {
					g_print("ERROR. Reading struct TM from string %s failed\n", new_text);
				} else {
					if (tm.tm_year < 100) {
						if (tm.tm_year < 40) {
							tm.tm_year += 2000;
						} else {
							tm.tm_year += 1900;
						}
					}
					char buffer[21];
					sprintf(buffer, "%d-%d-%d 00:00:00", tm.tm_year, tm.tm_mon, tm.tm_mday);
					//g_print("DATE: %s\n", buffer);

					//strptime(buffer, "%Y-%m-%d %H:%M:%S", &tm);
					//g_print("DATE: %d-%d-%d\n", tm.tm_year, tm.tm_mon, tm.tm_mday);
					tm.tm_year -= 1900; //struct tm year starts from 1900
					tm.tm_mon -= 1; //struct tm mount start with 0 for January
					tm.tm_isdst = 0; //should not use summer time flag 

					//dont forget to add zero to time - or we will have bugs!
					tm.tm_sec = 0; 
					tm.tm_min = 0;
					tm.tm_hour = 0;	
					
					time_t secons = mktime(&tm);
					int err = stroybat_set_smeta_date(smeta->uuid, secons);
					if (err){
						g_print("Error to change smeta date! Err: %d\n", err);
					} else {
						char date[11];
						//15.06.2009
						strftime(date, 32, "%d.%m.%Y", &tm);  						
						gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, date, -1);
					} 
				}	
				break;
			}			
		case COLUMN_ZAKAZCHIK:
			{
				g_print("Set Smeta Zakazchik to: %s\n", new_text);
				int err = stroybat_set_smeta_zakazchik(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta zakazchik! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		case COLUMN_PODRIADCHIK:
			{
				g_print("Set Smeta Podriadchik to: %s\n", new_text);
				int err = stroybat_set_smeta_podriadchik(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta podriadchik! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		case COLUMN_OBIEKT:
			{
				g_print("Set Smeta Obiekt to: %s\n", new_text);
				int err = stroybat_set_smeta_obiekt(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta obiekt! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		case COLUMN_RABOTI:
			{
				g_print("Set Smeta Raboti to: %s\n", new_text);
				int err = stroybat_set_smeta_raboti(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta raboti! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		case COLUMN_OSNOVANIYE:
			{
				g_print("Set Smeta Osnovaniye to: %s\n", new_text);
				int err = stroybat_set_smeta_osnovaniye(smeta->uuid, new_text);
				if (err){
					g_print("Error to change smeta osnovaniye! Err: %d\n", err);
				} else {
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);
				} 
				break;
			}			
		default: break;
	}
}

void gstroybat_smeta_table_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	
	GtkWidget* mainWindow = g_object_get_data(G_OBJECT(treeview), "mainWindow");
	GtkWidget* addServiceButton = g_object_get_data(G_OBJECT(mainWindow), "addServiceButton");
	GtkWidget* addProductButton = g_object_get_data(G_OBJECT(mainWindow), "addProductButton");
	GtkWidget* printButton = g_object_get_data(G_OBJECT(mainWindow), "printButton");
	GtkWidget* stroybatRemoveButton = g_object_get_data(G_OBJECT(mainWindow), "stroybatRemoveButton");

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatSmeta *smeta;
		gtk_tree_model_get(model, &iter, SMETA_POINTER, &smeta, -1); 
		if (smeta) {
			gstroybat_items_table_model_update(smeta);
			
			gtk_widget_set_sensitive(stroybatRemoveButton, true);
			g_object_set_data(G_OBJECT(stroybatRemoveButton), "StroybatSmeta", smeta);
			
			gtk_widget_set_sensitive(printButton, true);
			g_object_set_data(G_OBJECT(printButton), "StroybatSmeta", smeta);
			
			gtk_widget_set_sensitive(addProductButton, true);
			g_object_set_data(G_OBJECT(addProductButton), "StroybatSmeta", smeta);
			
			gtk_widget_set_sensitive(addServiceButton, true);
			g_object_set_data(G_OBJECT(addServiceButton), "StroybatSmeta", smeta);
		} else {
			g_print("No smeta data in row\n");
		}
	} else {
		gtk_widget_set_sensitive(stroybatRemoveButton, false);
		g_object_set_data(G_OBJECT(stroybatRemoveButton), "StroybatSmeta", NULL);
		
		gtk_widget_set_sensitive(printButton, false);
		g_object_set_data(G_OBJECT(printButton), "StroybatSmeta", NULL);
		
		gtk_widget_set_sensitive(addProductButton, false);
		g_object_set_data(G_OBJECT(addProductButton), "StroybatSmeta", NULL);
		
		gtk_widget_set_sensitive(addServiceButton, false);
		g_object_set_data(G_OBJECT(addServiceButton), "StroybatSmeta", NULL);
	}
}

void gstroybat_ask_to_remove_smeta_responce(GtkDialog *dialog, gint arg1, gpointer user_data){
	if (arg1 == 1) {
		g_print("Remove smeta button pushed\n");
		GtkListStore *store = user_data;
		StroybatSmeta *smeta = g_object_get_data(G_OBJECT(dialog), "StroybatSmeta");
		int err = stroybat_smeta_remove_all(smeta->uuid);
		if (err) {
			g_print("Error to remove smeta! Err: %d\n", err);
		}
		else {
			gstroybat_smeta_table_model_update(store, search);
		}
	}
	gtk_window_destroy(GTK_WINDOW(dialog));
}

void gstroybat_ask_to_remove_smeta(GtkListStore *store, StroybatSmeta *smeta, GtkWidget *window) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"Удалить смету %s?", smeta->title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");
	gtk_dialog_add_button(GTK_DIALOG(dialog), "УДАЛИТЬ", 1);
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", 0);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);
	g_object_set_data(G_OBJECT(dialog), "StroybatSmeta", smeta);
	g_signal_connect (dialog, "response", G_CALLBACK (gstroybat_ask_to_remove_smeta_responce), store);
	gtk_widget_show(dialog);
}

void gstroybat_smeta_remove_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Remove button clicked\n");
	
	GtkListStore *store = user_data;
	StroybatSmeta *smeta = g_object_get_data(G_OBJECT(button), "StroybatSmeta");
	
	if (smeta) {
		GtkWidget* mainWindow = g_object_get_data(G_OBJECT(button), "mainWindow");
		gstroybat_ask_to_remove_smeta(store, smeta, mainWindow);
	} else {
		g_print("Error to get smeta data!\n");
	}
}

void gstroybat_smeta_add_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Add button clicked\n");
	
	GtkListStore *store = user_data;
	StroybatSmeta *smeta = stroybat_smeta_new();
	if (smeta) {
		gstroybat_add_smeta_to_store(store, smeta);
	} else {
		g_print("Error to add new smeta!\n");
	}
}

void gstroybat_smeta_print_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Print button clicked\n");
	
	GtkListStore *store = user_data;
	StroybatSmeta *smeta = g_object_get_data(G_OBJECT(button), "StroybatSmeta");
	
	if (smeta) {
		remove("tmp.xlsx");
		cp("Template.xlsx", "tmp.xlsx", 0);
		stroybat_smeta_create_xlsx(smeta, "tmp.xlsx");
		openFile("tmp.xlsx");
	} else {
		g_print("Error to get smeta data!\n");
	}
}

void gstroybat_smeta_search_changed(GtkWidget *widget, gpointer user_data){
	GtkListStore *store = user_data;

	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
	const char *_search = gtk_entry_buffer_get_text(buffer);
	g_print("Search has changed to: %s\n", _search);
	search = (char *)_search;

	if (strlen(search) > 2) {
		gstroybat_smeta_table_model_update(store, search);	
	}

	if (strlen(search) == 0) {
		search = NULL;
		gstroybat_smeta_table_model_update(store, search);	
	}
}


GtkWidget *gstroybat_smeta_table_view_header(GtkListStore *store, GtkWidget* mainWindow){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	GtkWidget *search = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(search), "Поиск: название, заказчик, подрядчик, работы, объект");
	g_signal_connect (search, "changed", G_CALLBACK (gstroybat_smeta_search_changed), store);
	g_signal_connect (search, "insert-at-cursor", G_CALLBACK (gstroybat_smeta_search_changed), store);	
	gtk_box_append(GTK_BOX(header), search);	
	gtk_widget_set_hexpand(search, TRUE);
	
	GtkWidget *plusButton = gtk_button_new_with_label("+");
	g_signal_connect(plusButton, "clicked", (GCallback)gstroybat_smeta_add_button_pushed, store);
	gtk_box_append(GTK_BOX(header), plusButton);	
	
	GtkWidget* stroybatRemoveButton = gtk_button_new_with_label("-");
	g_object_set_data(G_OBJECT(mainWindow), "stroybatRemoveButton", stroybatRemoveButton);
	g_signal_connect(stroybatRemoveButton, "clicked", (GCallback)gstroybat_smeta_remove_button_pushed, store);
	gtk_widget_set_sensitive(stroybatRemoveButton, false);
	gtk_box_append(GTK_BOX(header), stroybatRemoveButton);	

	GtkWidget* printButton = gtk_button_new_with_label("🖨 Рапечатать");
	g_object_set_data(G_OBJECT(mainWindow), "printButton", printButton);
	g_signal_connect(printButton, "clicked", (GCallback)gstroybat_smeta_print_button_pushed, store);
	gtk_widget_set_sensitive(printButton, false);
	gtk_box_append(GTK_BOX(header), printButton);		

	return header;
}

GtkWidget *gstroybat_smeta_table_view_new(GtkWidget* mainWindow){
	
	GtkListStore *store = gstroybat_smeta_table_model_new();
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	
	//GtkWidget *header_title = gtk_label_new("Список смет");
	//gtk_box_append(GTK_BOX(box), gtk_label_new(""));	
	//gtk_box_append(GTK_BOX(box), header_title);	

	gtk_box_append(GTK_BOX(box), gstroybat_smeta_table_view_header(store, mainWindow));	

	GtkWidget *window = gtk_scrolled_window_new();
	gtk_widget_set_size_request (GTK_WIDGET(window), 900, 200);	
	gtk_widget_set_vexpand(window, TRUE);
	gtk_box_append(GTK_BOX(box), window);

	gstroybat_smeta_table_model_update(store, NULL);

	smetaTable = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(smetaTable), true);
	g_signal_connect(smetaTable, "row-activated", (GCallback) gstroybat_smeta_table_view_row_activated, store);
	g_object_set_data(G_OBJECT(smetaTable), "mainWindow", mainWindow);

	const char *column_titles[] = {"Наименование", "Дата", "Сумма", "Заказчик", "Подрядчик", "Объект", "Работы", "Основание"};

	int i;
	for (i = 0; i < N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
		g_signal_connect(renderer, "edited", (GCallback) gstroybat_smeta_table_cell_edited_callback, smetaTable);
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
			case COLUMN_TOTAL_PRICE: 
				{
					renderer = gtk_cell_renderer_text_new(); 
					column = gtk_tree_view_column_new_with_attributes(column_titles[i], renderer, "text", i,  NULL);
					break;
				};
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(smetaTable), column);	
	}

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(window), smetaTable);
	

	return box;
}

