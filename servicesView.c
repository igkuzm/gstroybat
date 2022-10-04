/**
 * File              : servicesView.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 04.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "gstroybat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "itemsTableModel.h"
	
void service_add_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Add button clicked\n");
	gstroybat_items_list_new(selectedSmeta, servicesViewStore, STROYBAT_SERVICES);
}

void service_remove_button_pushed(GtkButton *button, gpointer user_data){
	g_print("Remove button clicked\n");
	ask_to_remove_item(selectedService);
}

GtkWidget *services_view_header(){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	//title
	servicesLabel = gtk_label_new("Работы:");
	gtk_widget_set_hexpand(servicesLabel, TRUE);
	//gtk_box_append(GTK_BOX(header), servicesLabel);	
	gtk_container_add(GTK_CONTAINER(header), servicesLabel);

	//add button
	serviceAddButton = gtk_button_new_with_label("+");
	g_signal_connect(serviceAddButton, "clicked", (GCallback)service_add_button_pushed, NULL);
	//gtk_box_append(GTK_BOX(header), serviceAddButton);	
	gtk_container_add(GTK_CONTAINER(header), serviceAddButton);

	//remove button
	serviceRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(serviceRemoveButton, false);
	g_signal_connect(serviceRemoveButton, "clicked", (GCallback)service_remove_button_pushed, NULL);
	//gtk_box_append(GTK_BOX(header), serviceRemoveButton);	
	gtk_container_add(GTK_CONTAINER(header), serviceRemoveButton);

	return header;
}

GtkWidget *services_view_new(){
	//create new Store
	servicesViewStore = table_model_new();

	return items_view_new(services_view_header(), servicesViewStore, STROYBAT_SERVICES);
}
