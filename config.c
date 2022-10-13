/**
 * File              : config.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 13.10.2022
 * Last Modified Date: 13.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include <time.h>

#define CONFIG_FILE "stroybat.ini"

void save_window_state (GtkWidget *widget, GtkAllocation *allocation, gpointer userdata){
	const char * window_name = userdata;
	window_save_state_to_config(window_name, allocation->x, allocation->y, allocation->width, allocation->height);
}

void window_save_state_to_config(const char * window_name, int x, int y, int width, int height){
	GKeyFile *key_file = g_key_file_new ();
	g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL);
	g_key_file_set_integer(key_file, window_name, "x",  x);
	g_key_file_set_integer(key_file, window_name, "y",  y);
	g_key_file_set_integer(key_file, window_name, "width",  width);
	g_key_file_set_integer(key_file, window_name, "height", height);
	g_key_file_save_to_file(key_file, CONFIG_FILE, NULL);
}

void window_restore_state_from_config(GtkWindow * window, const char * window_name, int default_width, int default_height){
	//int x = -1, y = -1;
	int width = default_width, height = default_height;
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL)){
		width  = g_key_file_get_integer(key_file, window_name, "width", NULL); 
		height = g_key_file_get_integer(key_file, window_name, "height", NULL);
		//x = g_key_file_get_integer(key_file, window_name, "x", NULL);
		//y = g_key_file_get_integer(key_file, window_name, "y", NULL);

		if (width < 100 || height < 100){
			width = default_width;	
			height = default_height;
		}
			
	}	
	gtk_window_set_default_size(window, width, height);
	//if (x > -1 && y > -1)
		//gtk_window_move(window, x, y);
}

void save_token_to_config(const char * token, time_t expires, const char * reftoken){
	GKeyFile *key_file = g_key_file_new ();
	g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL);
	g_key_file_set_value(key_file, "YandexDisk", "token", token);
	g_key_file_set_int64(key_file, "YandexDisk", "expires", expires);
	g_key_file_set_value(key_file, "YandexDisk", "reftoken", reftoken);
	g_key_file_save_to_file(key_file, CONFIG_FILE, NULL);	
}

char * token_from_config(){
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL)){
		gchar * token = g_key_file_get_value(key_file, "YandexDisk", "token", NULL);
		if (token){
			char * ret = malloc(64);
			if (!ret)
				return strdup("");
			strncpy(ret, token, 63);
			ret[63] = 0;
			return ret;
		}
	}	
	return strdup("");
}
