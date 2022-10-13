/**
 * File              : mainWindow.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"

void gstroybat_application_on_activate (GtkApplication *app, gpointer user_data) {

	//add main menu
	gstroybat_application_menu(app);

	//add main window
	GtkWidget * mainWindow = gtk_application_window_new(app);
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (mainWindow), TRUE);
	g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gstroybat_application_on_deactivate), app); //quit application on window destroy
	window_restore_state_from_config(GTK_WINDOW(mainWindow), "mainWindow", 1200, 680);
	g_signal_connect(G_OBJECT(mainWindow), "size-allocate", G_CALLBACK(save_window_state), "mainWindow"); //save window state
	gtk_window_set_title(GTK_WINDOW(mainWindow), "GStroyBat: Список смет");
	g_object_set_data(G_OBJECT(app), "mainWindow", mainWindow);

	//main window overlay
	GtkWidget *overlay = gtk_overlay_new(); 
	gtk_container_add (GTK_CONTAINER (mainWindow), overlay);

	//split window verticaly
	GtkWidget *hsplitter = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	//gtk_window_set_child(GTK_WINDOW(mainWindow), hsplitter);
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), hsplitter);

	//add box to left
	GtkWidget *leftbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	//gtk_paned_set_start_child(GTK_PANED(hsplitter), leftbox);
	gtk_paned_pack1(GTK_PANED(hsplitter), leftbox, true, true);

	//allocate searchview
	GtkWidget * smetaViewSearch = gtk_entry_new();
	g_object_set_data(G_OBJECT(app), "smetaViewSearch", smetaViewSearch);
	
	//add smetaView to leftbox 
	//gtk_box_append(GTK_BOX(leftbox), smeta_view_new());	
	gtk_container_add(GTK_CONTAINER(leftbox), smeta_view_new(G_OBJECT(app)));

	//add bottom to leftbox
	GtkWidget *lbottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	//gtk_box_append(GTK_BOX(leftbox), lbottom);	
	gtk_container_add(GTK_CONTAINER(leftbox), lbottom);
	
	gtk_entry_set_placeholder_text(GTK_ENTRY(smetaViewSearch), "Поиск: название, заказчик, подрядчик, работы, объект");
	//gtk_box_append(GTK_BOX(lbottom), smetaViewSearch);	
	gtk_container_add(GTK_CONTAINER(lbottom), smetaViewSearch);
	gtk_widget_set_hexpand(smetaViewSearch, TRUE);	
	
	//add box to right
	GtkWidget *rightbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	//gtk_paned_set_end_child(GTK_PANED(hsplitter), rightbox);
	gtk_paned_pack2(GTK_PANED(hsplitter), rightbox, true, true);

	//add horysontaly split to rightbox 
	GtkWidget *vsplitter = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	//gtk_box_append(GTK_BOX(rightbox), vsplitter);	
	gtk_container_add(GTK_CONTAINER(rightbox), vsplitter);

	//add bottom to rightbox
	GtkWidget *rbottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	//gtk_box_append(GTK_BOX(rightbox), rbottom);	
	gtk_container_add(GTK_CONTAINER(rightbox), rbottom);
	
	GtkWidget * totalPriceLabel = gtk_label_new("Итого:");
	gtk_widget_set_hexpand(totalPriceLabel, TRUE);
	//gtk_box_append(GTK_BOX(rbottom), totalPriceLabel);	
	gtk_container_add(GTK_CONTAINER(rbottom), totalPriceLabel);
	g_object_set_data(G_OBJECT(app), "totalPriceLabel", totalPriceLabel);
	
	GtkWidget * makeExcelButton = gtk_button_new_with_label("Excel");
	g_signal_connect(makeExcelButton, "clicked", (GCallback)make_excel, app);
	//gtk_box_append(GTK_BOX(rbottom), makeExcelButton);		
	gtk_container_add(GTK_CONTAINER(rbottom), makeExcelButton);
	g_object_set_data(G_OBJECT(app), "makeExcelButton", makeExcelButton);
	
	//add servicesView to top of split
	//gtk_paned_set_end_child(GTK_PANED(vsplitter), services_view_new());	
	gtk_paned_pack1(GTK_PANED(vsplitter), services_view_new(G_OBJECT(app)), true, true);
	
	//add materialsView to bottom of split
	//gtk_paned_set_start_child(GTK_PANED(vsplitter), materials_view_new());	
	gtk_paned_pack2(GTK_PANED(vsplitter), materials_view_new(G_OBJECT(app)), true, true);

	//add notification toast
	GtkWidget * toast = gtk_toast_new(); 
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), toast);
	g_object_set_data(G_OBJECT(app), "mainWindow_toast", toast);
	
	//show main window
	//gtk_window_present (GTK_WINDOW (mainWindow));
	gtk_widget_show_all(mainWindow);
}

void gstroybat_application_on_deactivate (GtkWidget *window, gpointer userdata) {
	g_application_quit (userdata);
}

