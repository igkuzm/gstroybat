/**
 * File              : servicesView.c
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
	
void service_add_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Add button clicked\n");
	GObject *app = userdata;
	StroybatSmeta * smeta = g_object_get_data(app, "selectedSmeta"); 	
	GtkListStore  * store = g_object_get_data(app, "servicesViewStore"); 	
	if (!smeta){
		g_print("Error! No smeta!\n");
		return;	
	}
	g_print("Add for smeta: %s\n", smeta->uuid);
	gstroybat_items_list_new(app, smeta, store, STROYBAT_SERVICES);
}

void service_remove_button_pushed(GtkButton *button, gpointer userdata){
	g_print("Remove button clicked\n");
	GObject *app = userdata;
	StroybatItem * item = g_object_get_data(app, "selectedService");	
	ask_to_remove_item(item);
}

GtkWidget *services_view_header(GObject *app){
	GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	//title
	servicesLabel = gtk_label_new("Работы:");
	gtk_widget_set_hexpand(servicesLabel, TRUE);
	//gtk_box_append(GTK_BOX(header), servicesLabel);	
	gtk_container_add(GTK_CONTAINER(header), servicesLabel);

	//add button
	serviceAddButton = gtk_button_new_with_label("+");
	g_signal_connect(serviceAddButton, "clicked", (GCallback)service_add_button_pushed, app);
	//gtk_box_append(GTK_BOX(header), serviceAddButton);	
	gtk_container_add(GTK_CONTAINER(header), serviceAddButton);

	//remove button
	serviceRemoveButton = gtk_button_new_with_label("-");
	gtk_widget_set_sensitive(serviceRemoveButton, false);
	g_signal_connect(serviceRemoveButton, "clicked", (GCallback)service_remove_button_pushed, app);
	//gtk_box_append(GTK_BOX(header), serviceRemoveButton);	
	gtk_container_add(GTK_CONTAINER(header), serviceRemoveButton);

	return header;
}

GtkWidget *services_view_new(GObject *app){
	//create new Store
	GtkListStore *store = table_model_new();
	g_object_set_data(app, "servicesViewStore", store);	

	return items_view_new(services_view_header(app), app, store, STROYBAT_SERVICES);
}
