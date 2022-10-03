/**
 * File              : smetaEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.02.2022
 * Last Modified Date: 03.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "gstroybat.h"
#include <time.h>

#define STR(...) ({char str[BUFSIZ]; sprintf(str, __VA_ARGS__); str;})
#define STRTIME(time) ({char str[11]; struct tm *tm = localtime(&time); strftime(str, 11, "%d.%m.%Y", tm); str;})
#define STRINT(n) ({char str[16]; sprintf(str, "%d", n); str;})

enum {
  COLUMN_TITLE = 0,
  COLUMN_DATE,
  COLUMN_ZAKAZCHIK,
  COLUMN_PODRIADCHIK,
  COLUMN_OBIEKT,
  COLUMN_RABOTI,
  COLUMN_OSNOVANIYE,
  N_COLUMNS
};

void smeta_edit_on_deactivate (GtkWidget *widget, gpointer user_data) {
}

void smeta_edit_changed(GtkWidget *widget, gpointer user_data){
	StroybatSmeta *smeta = user_data;
	char * key = g_object_get_data(G_OBJECT(widget), "key");
	GtkEntryBuffer * buf = gtk_entry_get_buffer(GTK_ENTRY(widget));
	const char *value = gtk_entry_buffer_get_text(buf);
	if (!strcmp("date", key)){ //if date entry
		g_print("Set Smeta Date to: %s\n", value);
		struct tm tm;
		int c = sscanf(value, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);	
		if (c != 3) {
			g_print("ERROR. Reading struct TM from string %s failed\n", value);
			return;
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
			tm.tm_year -= 1900; //struct tm year starts from 1900
			tm.tm_mon -= 1; //struct tm mount start with 0 for January
			tm.tm_isdst = 0; //should not use summer time flag 

			//dont forget to add zero to time - or we will have bugs!
			tm.tm_sec = 0; 
			tm.tm_min = 0;
			tm.tm_hour = 0;	
			
			time_t secons = mktime(&tm);
			sprintf(buffer, "%ld", secons);
			value=buffer;
		}
	}

	g_print("set value: %s for key:%s\n", value, key);
	if (!stroybat_smeta_set_value_for_key(database, smeta->uuid, value, key))
		smeta_view_table_model_update();
}

GtkWidget * smeta_edit_widget(
		StroybatSmeta *smeta, 
		const char * value,
		const char * key,
		const char * description
){
	//make horizontal box
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

	//add entry
	GtkWidget *entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(entry), "key", (void *)key);
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry), description);
	GtkEntryBuffer * buf = gtk_entry_get_buffer(GTK_ENTRY(entry));
	gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(buf), value, strlen(value));
	g_signal_connect (entry, "changed", G_CALLBACK (smeta_edit_changed), smeta);
	/*g_signal_connect (entry, "insert-at-cursor", G_CALLBACK (smeta_view_search_changed), store);	*/

	gtk_box_append(GTK_BOX(box), entry);

	//space
	/*GtkWidget * space = gtk_label_new("");*/
	/*gtk_widget_set_hexpand(space, TRUE);*/
	/*gtk_box_append(GTK_BOX(box), space);		*/

	//add description label
	GtkWidget *label = gtk_label_new(description);
	gtk_box_append(GTK_BOX(box), label);

	return box;
}

void smeta_edit_new(StroybatSmeta *smeta){

	//create new modal window 
	GtkWidget *win = gtk_window_new();
	//g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(smeta_edit_on_deactivate), smeta);
	gtk_window_set_modal(GTK_WINDOW(win), true);
	gtk_window_set_title(GTK_WINDOW(win), "Редактирование сметы");
	
	//create main container
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_window_set_child(GTK_WINDOW(win), box);
	
	//add widgets
	const char *titles[] = {"Наименование", "Дата", "Заказчик", "Подрядчик", "Объект", "Работы", "Основание"};
	const char *values[] = {smeta->title, STRTIME(smeta->date), smeta->zakazchik, smeta->podriadchik, smeta->obiekt, smeta->raboti, smeta->osnovaniye};
	const char *keys[]   = {"title", "date", "zakazchik", "podriadchik", "obiekt", "raboti", "osnovanie"};
	for (int i = 0; i < N_COLUMNS; ++i) {
		GtkWidget *widget = smeta_edit_widget(smeta, values[i], keys[i], titles[i]);
		gtk_box_append(GTK_BOX(box), widget);
	}

	//show window
	gtk_window_present(GTK_WINDOW(win));
}

