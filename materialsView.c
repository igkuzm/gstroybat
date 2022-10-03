/**
 * File              : materialsView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 03.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "gstroybat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "itemsTableModel.h"

void material_add_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Add button clicked\n");
	gstroybat_items_list_new(selectedSmeta, materialsViewStore, STROYBAT_MATERIALS);
}

void material_remove_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Remove button clicked\n");
	ask_to_remove_item(selectedMaterial);
}

GtkWidget *materials_view_header(){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	//title
	materialsLabel = gtk_label_new("Материалы:");
	gtk_widget_set_hexpand(materialsLabel, TRUE);
	gtk_box_append(GTK_BOX(header), materialsLabel);	

	//add button
	materialAddButton = gtk_button_new_with_label("+");
	g_signal_connect(materialAddButton, "clicked", (GCallback)material_add_button_pushed, materialsViewStore);
	gtk_box_append(GTK_BOX(header), materialAddButton);		
	
	//remove button
	materialRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(materialRemoveButton, false);
	g_signal_connect(materialRemoveButton, "clicked", (GCallback)material_remove_button_pushed, materialsViewStore);
	gtk_box_append(GTK_BOX(header), materialRemoveButton);	

	return header;
}

GtkWidget *materials_view_new(){
	//create new Store
	materialsViewStore = table_model_new();
	
	return items_view_new(materials_view_header(), materialsViewStore, STROYBAT_MATERIALS);
}
