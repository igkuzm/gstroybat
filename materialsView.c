/**
 * File              : materialsView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 07.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "gstroybat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "itemsTableModel.h"

void material_add_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Add button clicked\n");
	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta"); 		
	GtkListStore  * store = g_object_get_data(app, "materialsViewStore"); 	
	if (!smeta){
		g_print("Error! No smeta!\n");
		return;	
	}
	g_print("Add for smeta: %s\n", smeta->uuid);	
	gstroybat_items_list_new(app, smeta, store, STROYBAT_MATERIALS);
}

void material_remove_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Remove button clicked\n");
	GObject *app = userdata;
	StroybatItem * item = g_object_get_data(app, "selectedMaterial");	
	ask_to_remove_item(item);
}

GtkWidget *materials_view_header(GObject *app){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	//title
	materialsLabel = gtk_label_new("Материалы:");
	gtk_widget_set_hexpand(materialsLabel, TRUE);
	/*gtk_box_append(GTK_BOX(header), materialsLabel);	*/
	gtk_container_add(GTK_CONTAINER(header), materialsLabel);

	//add button
	materialAddButton = gtk_button_new_with_label("+");
	g_signal_connect(materialAddButton, "clicked", (GCallback)material_add_button_pushed, app);
	/*gtk_box_append(GTK_BOX(header), materialAddButton);		*/
	gtk_container_add(GTK_CONTAINER(header), materialAddButton);
	
	//remove button
	materialRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(materialRemoveButton, false);
	g_signal_connect(materialRemoveButton, "clicked", (GCallback)material_remove_button_pushed, app);
	/*gtk_box_append(GTK_BOX(header), materialRemoveButton);	*/
	gtk_container_add(GTK_CONTAINER(header), materialRemoveButton);

	return header;
}

GtkWidget *materials_view_new(GObject *app){
	//create new Store
	GtkListStore *store = table_model_new();
	g_object_set_data(app, "materialsViewStore", store);	
	
	return items_view_new(materials_view_header(app), app, store, STROYBAT_MATERIALS);
}
