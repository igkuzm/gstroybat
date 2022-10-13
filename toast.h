/**
 * File              : toast.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 13.10.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef GTK_TOASH_H
#define GTK_TOASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#define GTK_TOAST(p) ((GtkRevealer *)p)

typedef GtkRevealer GtkToast;

GtkWidget * gtk_toast_new();

void gtk_toast_show_message(
		GtkToast *toast,
		const char * message,
		int seconds,
		void * cancel_data,
		void (*on_cancel)(GtkToast *toast, void * cancel_data),
		void * commited_data,
		void (*on_commited)(GtkToast *toast, void * commited_data)
		);

#ifdef __cplusplus
}
#endif

#endif //GTK_TOASH_H	
