/**
 * File              : YDConnect.cc
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.10.2022
 * Last Modified Date: 20.03.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include "webview.h"
#include "stroybat/kdata2/cYandexDisk/cYandexDisk.h"
#include <cstddef>
#include <cstdio>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

webview_t w;

struct YDConnect_d{
	void *user_data;
	int (*callback)(void *user_data, char *token, time_t expires, char *reftoken, char *error);
};

void searchCode(const char *seq, const char *req,  void *arg){
	YDConnect_d *d = (YDConnect_d *)arg;

	char *error = NULL;
	char *code = c_yandex_disk_verification_code_from_html(req, &error);
	if(error){
		if (d->callback)
			d->callback(d->user_data, NULL, 0, NULL, error);
		free(error);
		return;
	}
	
	if (code){
		printf("CODE: %s\n", code);
		char devicename[BUFSIZ];
		sprintf(devicename, "%s.%s", g_get_host_name(), g_get_user_name());
		c_yandex_disk_get_token(code, "43cf5841c3014cdf94877c9653f0b334", "adc6cafaa56a43f98ceb29b28117a0b8", devicename, d->user_data, d->callback);
		free(code);
		webview_hide(w);
	}
}

void webview_dispatch_fn(webview_t w, void *arg){
	
	YDConnect_d *d = (YDConnect_d *)arg;
	
	webview_set_title(w, "Yandex Disk");
	webview_set_size(w, 400, 600, WEBVIEW_HINT_NONE);

	webview_bind(w, "searchCode", searchCode, d);
	webview_init(w, "window.onload = function(){ searchCode(document.body.innerHTML); }");

	webview_hide(w);
}

void YDConnect(
		void *user_data, 
		int (*callback)(
			void *user_data, 
			char *token, 
			time_t expires, 
			char *reftoken, 
			char *error
			)
		) 
{
	if (w){
		webview_show(w);
	} else {
		w = webview_create(0, NULL);
		YDConnect_d *d = (YDConnect_d *)malloc(sizeof(YDConnect_d));
			d->user_data = user_data;
			d->callback = callback;
		
		webview_dispatch(w, webview_dispatch_fn, d);	
	}
}

void YDShow(){
	char *requestURI = c_yandex_disk_url_to_ask_for_verification_code("43cf5841c3014cdf94877c9653f0b334", NULL); 
	webview_navigate(w, requestURI);
	free(requestURI);
	
	webview_show(w);
}
