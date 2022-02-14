/**
 * File              : mainWindow.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 10.02.2022
 * Last Modified Date: 10.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include <gtk/gtk.h>

void gstroybat_application_on_activate (GtkApplication *app);

void gstroybat_application_on_deactivate (GtkWidget *widget, gpointer userData);
