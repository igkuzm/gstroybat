/**
 * File              : YDConnect.cc
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.10.2022
 * Last Modified Date: 01.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "gstroybat.h"
#include "webview/webview.h"
#include "stroybat/kdata/cYandexDisk/cYandexDisk.h"
#include <cstdio>
#include <stddef.h>
#include <string.h>

struct YDConnect_d{
	void *user_data;
	int (*callback)(void *user_data, char *token, time_t expires, char *reftoken, char *error);
	webview_t w;
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
		webview_terminate(d->w);
	}
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
	webview_t w = webview_create(0, NULL);
	webview_set_title(w, "Yandex Disk");
	webview_set_size(w, 400, 600, WEBVIEW_HINT_NONE);

	YDConnect_d d = {
		.user_data = user_data,
		.callback = callback,
		.w = w
	};

	webview_bind(w, "searchCode", searchCode, &d);
	webview_init(w, "window.onload = function(){ searchCode(document.body.innerHTML); }");

	char *error = NULL;
	char *requestURI = c_yandex_disk_url_to_ask_for_verification_code("43cf5841c3014cdf94877c9653f0b334", &error); 
	if(error){
		if (callback)
			callback(user_data, NULL, 0, NULL, error);
		free(error);
		free(requestURI);
		webview_destroy(w);
		return;
	}
	
	webview_navigate(w, requestURI);
	webview_run(w);

	free(requestURI);

	webview_destroy(w);
}
