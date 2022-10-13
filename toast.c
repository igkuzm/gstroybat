/**
 * File              : toast.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 13.10.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "toast.h"
#include "glibconfig.h"

GtkWidget * gtk_toast_new(){
	//add notification revealer
	GtkWidget *toast = gtk_revealer_new();
	gtk_widget_set_valign(GTK_WIDGET(toast), GTK_ALIGN_START);
	gtk_widget_set_halign(GTK_WIDGET(toast), GTK_ALIGN_CENTER);
	gtk_revealer_set_transition_type (GTK_REVEALER (toast), GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
	gtk_revealer_set_transition_duration (GTK_REVEALER (toast), 500);
	gtk_revealer_set_reveal_child (GTK_REVEALER (toast), FALSE);
	
	//add notification box
	GtkWidget * box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	GtkStyleContext *context = gtk_widget_get_style_context(box);
	gtk_style_context_add_class(context, "app-notification");
	gtk_container_add (GTK_CONTAINER (toast), box);	
	g_object_set_data(G_OBJECT(toast), "box", box);

	//notification label
	GtkWidget * label = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, TRUE, 18);
	g_object_set_data(G_OBJECT(toast), "label", label);
	
	//cancel button
	GtkWidget * cancelButton = gtk_button_new_with_label("отмена");
	gtk_box_pack_start(GTK_BOX(box), cancelButton, FALSE, TRUE, 1);
	g_object_set_data(G_OBJECT(toast), "cancelButton", cancelButton);
	
	//close button
	GtkWidget * closeButton = gtk_button_new_with_label("x");
	gtk_button_set_relief(GTK_BUTTON(closeButton), GTK_RELIEF_NONE);
	gtk_widget_set_receives_default(closeButton, TRUE);
	gtk_box_pack_start(GTK_BOX(box), closeButton, FALSE, TRUE, 1);
	g_object_set_data(G_OBJECT(toast), "closeButton", closeButton);
	
	return GTK_WIDGET(toast);
}

struct gtk_toast_t{
	GtkRevealer *toast;
	void * cancel_data;
	void (*on_cancel)(GtkToast *toast, void * cancel_data);
	void * commited_data;
	void (*on_commited)(GtkToast *toast, void * commited_data);	
	gboolean * canceled;
};

void gtk_toast_cancel(GtkWidget *widget, gpointer userdata) {
	struct gtk_toast_t *d = userdata;
	d->canceled[0] = TRUE;
	gtk_revealer_set_reveal_child (d->toast, FALSE);
	if (d->on_cancel)
		d->on_cancel(d->toast, d->cancel_data);
}

void gtk_toast_close(GtkWidget *widget, gpointer userdata) {
	GtkRevealer * toast = userdata;
	gtk_revealer_set_reveal_child (toast, FALSE);
}

void gtk_toast_on_reveal(GtkWidget *widget, gpointer userdata) {
	GtkRevealer * toast = GTK_TOAST(widget);

	//set canceled to FALSE
	gboolean * canceled = userdata;
	*canceled = FALSE;
}

int gtk_toast_hide(gpointer userdata){
	struct gtk_toast_t *d = userdata;
	gtk_revealer_set_reveal_child (d->toast, FALSE);
	if (d->on_commited && !d->canceled[0])
		d->on_commited(d->toast, d->commited_data);
	return 0;
}

void gtk_toast_dismiss(gpointer userdata){
	struct gtk_toast_t *d = userdata;	
	//free(d->canceled);
	//free(d);	
}

void gtk_toast_show_message(
		GtkRevealer *toast,
		const char * message,
		int seconds,
		void * cancel_data,
		void (*on_cancel)(GtkRevealer *toast, void * cancel_data),
		void * commited_data,
		void (*on_commited)(GtkRevealer *toast, void * commited_data)
		)
{
	if (!toast) {
		g_error("GtkToast is NULL\n");
		return;
	}
	if (!message) {
		g_error("GtkToast: message is NULL\n");
		return;
	}	

	/*dont show cancel button if no callback*/
	GtkWidget * cancelButton = g_object_get_data(G_OBJECT(toast), "cancelButton");
	if (on_cancel)
		gtk_widget_show(cancelButton);
	else
		gtk_widget_hide(cancelButton);

	//set message
	GtkWidget * label = g_object_get_data(G_OBJECT(toast), "label");
	gtk_label_set_text(GTK_LABEL(label), message);

	//connect close button
	GtkWidget * closeButton = g_object_get_data(G_OBJECT(toast), "closeButton");
	g_signal_connect(G_OBJECT(closeButton), "clicked", G_CALLBACK(gtk_toast_close), toast);

	//create boole canceled
	gboolean * canceled = malloc(sizeof(gboolean));
	*canceled = FALSE;

	//create data struct
	struct gtk_toast_t *d = malloc(sizeof(struct gtk_toast_t));
	if (!d){
		gtk_revealer_set_reveal_child (GTK_REVEALER(toast), FALSE);
		return;
	}
	d->toast = toast;
	d->cancel_data = cancel_data;
	d->on_cancel = on_cancel;	
	d->commited_data = commited_data;
	d->on_commited = on_commited;
	d->canceled = canceled;

	//connect on_cancel
	if (on_cancel)
		g_signal_connect(G_OBJECT(cancelButton), "clicked", G_CALLBACK(gtk_toast_cancel), d);

	//connect on_reveal
	g_signal_connect_after(G_OBJECT(toast), "notify::child-revealed", G_CALLBACK(gtk_toast_on_reveal), canceled);

	//reveal child
	gtk_revealer_set_reveal_child(GTK_REVEALER(toast), TRUE);

	//hide child in n seconds
	g_timeout_add_full(100, seconds * 1000, gtk_toast_hide, d, gtk_toast_dismiss);
}
