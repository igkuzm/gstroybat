/**
 * File              : smetaView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 09.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "gstroybat.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gio/gio.h>

#define STR(...) ({char str[BUFSIZ]; sprintf(str, __VA_ARGS__); str;})

GtkListStore *smetaViewStore;

enum {
  COLUMN_TITLE = 0,
  SMETA_POINTER,
  N_COLUMNS
};

void smeta_view_add(GtkListStore *store, StroybatSmeta *smeta){
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
				COLUMN_TITLE, smeta->title, 
				SMETA_POINTER, smeta, 
	-1);
}

int smeta_view_fill(StroybatSmeta *smeta, void *data, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}
	GtkListStore *store = data;
	smeta_view_add(store, smeta);

	return 0;
}

GtkListStore *smeta_view_table_model_new(){
	GtkListStore *store = gtk_list_store_new(N_COLUMNS, 
			G_TYPE_STRING, //title 
			G_TYPE_POINTER); //smeta pointer

	return store;
}

gboolean smeta_view_table_model_free(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, gpointer data) {
	StroybatSmeta *smeta;
	gtk_tree_model_get(model, iter, SMETA_POINTER, &smeta, -1);	
	free(smeta);
	return false;
}

void smeta_view_table_model_update(char * search){
	gtk_tree_model_foreach (GTK_TREE_MODEL(smetaViewStore), smeta_view_table_model_free, NULL);
	gtk_list_store_clear(smetaViewStore);
	
	stroybat_smeta_get_all(DATABASE, search, smetaViewStore, smeta_view_fill);
}

void smeta_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata){
	g_print("Row activated\n");

	GObject *app = userdata;
	
	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "materialsLabel")), "Материалы:");	
	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "servicesLabel")), "Работы:");	
	gtk_label_set_text(GTK_LABEL(g_object_get_data(app, "totalPriceLabel")), "Итого:");	

	g_object_set_data(app, "selectedSmeta", NULL);
	gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "smetaEditButton")), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "smetaRemoveButton")), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "makeExcelButton")), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "materialAddButton")), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "serviceAddButton")), FALSE);

	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	
	if (gtk_tree_model_get_iter(model, &iter, path)) {
		StroybatSmeta *smeta;
		gtk_tree_model_get(model, &iter, SMETA_POINTER, &smeta, -1); 
		if (smeta) {
			g_object_set_data(app, "selectedSmeta", smeta);
			g_print("selected smeta: %s\n", smeta->uuid);

			table_model_update(app, smeta);
			
			gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "smetaEditButton")), TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "smetaRemoveButton")), TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "makeExcelButton")), TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "materialAddButton")), TRUE);
			gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "serviceAddButton")), TRUE);
			
			return;
		} 
	}
	g_print("No smeta selected\n");
}

void smeta_view_search_changed(GtkWidget *widget, gpointer user_data){
	GtkListStore *store = user_data;

	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
	const char *searchString = gtk_entry_buffer_get_text(buffer);
	g_print("Search has changed to: %s\n", searchString);
	
	if (strlen(searchString) > 2) {
		smeta_view_table_model_update((char*)searchString);	
	}

	if (strlen(searchString) == 0) {
		smeta_view_table_model_update(NULL);	
	}
}

void ask_to_remove_smeta_responce(GtkDialog *dialog, gint arg1, gpointer userdata){
	if (arg1 == 1) {
		g_print("Remove commited\n");

		GObject *app = userdata;
		StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta");		

		if (!smeta){
			g_print("Smeta is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		
		
		if (stroybat_smeta_remove_all(DATABASE, smeta->uuid)){
			g_print("Error to remove Item!\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}
			
		smeta_view_table_model_update(NULL);	
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ask_to_remove_smeta(GObject *app, StroybatSmeta * smeta) {
	if (!smeta){
		g_print("Smeta is NULL\n");
		return;
	}
	char *title = STR("Удалить смету %s?", smeta->title);
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
	g_signal_connect (dialog, "response", G_CALLBACK (ask_to_remove_smeta_responce), app);
	gtk_widget_show(dialog);
}

void smeta_edit_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Edit button clicked\n");
	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta"); 
	if(smeta)
		smeta_edit_new(smeta);
}

void smeta_add_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Add button clicked\n");

	StroybatSmeta *smeta = stroybat_smeta_new(DATABASE);
	smeta_view_table_model_update(NULL);	
	if (smeta)
		smeta_edit_new(smeta);
}

void smeta_remove_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Remove button clicked\n");

	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta"); 
	ask_to_remove_smeta(app, smeta);
}

GtkWidget *smeta_view_header(GObject *app){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

	GtkWidget * smetaEditButton = gtk_button_new_with_label("прав.");
	//gtk_box_append(GTK_BOX(header), smetaEditButton);	
	gtk_container_add(GTK_CONTAINER(header), smetaEditButton);
	gtk_widget_set_sensitive(smetaEditButton, FALSE);
	g_signal_connect(smetaEditButton, "clicked", (GCallback)smeta_edit_button_pushed, app);
	g_object_set_data(app, "smetaEditButton", smetaEditButton);
	
	GtkWidget * space = gtk_label_new("");
	gtk_widget_set_hexpand(space, TRUE);
	//gtk_box_append(GTK_BOX(header), space);	
	gtk_container_add(GTK_CONTAINER(header), space);
	
	GtkWidget * smetaAddButton = gtk_button_new_with_label("+");
	//gtk_box_append(GTK_BOX(header), smetaAddButton);		
	gtk_container_add(GTK_CONTAINER(header), smetaAddButton);
	g_signal_connect(smetaAddButton, "clicked", (GCallback)smeta_add_button_pushed, smetaViewStore);
	g_object_set_data(app, "smetaAddButton", smetaAddButton);
	
	GtkWidget * smetaRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(smetaRemoveButton, FALSE);
	//gtk_box_append(GTK_BOX(header), smetaRemoveButton);	
	gtk_container_add(GTK_CONTAINER(header), smetaRemoveButton);
	g_signal_connect(smetaRemoveButton, "clicked", (GCallback)smeta_remove_button_pushed, app);
	g_object_set_data(app, "smetaRemoveButton", smetaRemoveButton);
	
	return header;
}

void smeta_view_search_init(GObject *app){
	g_signal_connect (GTK_WIDGET(g_object_get_data(app, "smetaViewSearch")), "changed", G_CALLBACK (smeta_view_search_changed), smetaViewStore);
	g_signal_connect (GTK_WIDGET(g_object_get_data(app, "smetaViewSearch")), "insert-at-cursor", G_CALLBACK (smeta_view_search_changed), smetaViewStore);	
}

GtkWidget *smeta_view_new(GObject *app){
	
	smetaViewStore = smeta_view_table_model_new();
	
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	//init search view
	smeta_view_search_init(app);
	
	//gtk_box_append(GTK_BOX(box), smeta_view_header());	
	gtk_container_add(GTK_CONTAINER(box), smeta_view_header(app));

	//GtkWidget *window = gtk_scrolled_window_new();
	GtkWidget *window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request (GTK_WIDGET(window), 200, 600);	
	gtk_widget_set_vexpand(window, TRUE);
	//gtk_box_append(GTK_BOX(box), window);
	gtk_container_add(GTK_CONTAINER(box), window);

	smeta_view_table_model_update(NULL);

	GtkWidget *smetaView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(smetaViewStore));
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(smetaView), TRUE);
	g_object_set(smetaView, "activate_on_single_click", TRUE, NULL);	
	g_signal_connect(smetaView, "row-activated", (GCallback) smeta_view_row_activated, app);

	const char *column_titles[] = {"Список смет"};

	int i;
	for (i = 0; i < N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", FALSE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
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
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(smetaView), column);	
	}

	//gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(window), smetaView);
	gtk_container_add(GTK_CONTAINER(window), smetaView);

	return box;
}
