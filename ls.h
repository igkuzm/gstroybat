/**
 * File              : ls.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * File              : ls.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.02.2022
 * Last Modified Date: 22.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef k_lib_ls_h__
#define k_lib_ls_h__
#endif

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>

typedef enum k_lib_filetype_t {
	KLIBFILETYPE_FILE,
	KLIBFILETYPE_LINK,
	KLIBFILETYPE_DIR,
	KLIBFILETYPE_OTHER
} KLIBFILETYPE;

typedef struct k_lib_file_t {
	KLIBFILETYPE filetype;
	const char *filename;
	const char *filepath;
} KLibFile;

//run callback for every file in dir. to stop execution - return non zero from callback
int k_lib_ls(const char *dir, void *user_data, int (*callback)(KLibFile *file, void *user_data));

#define ls(dir, user_data, callback)	\
({	\
	int ___c = k_lib_ls(dir, user_data, callback);\
	___c;	\
})

#ifdef __cplusplus
}
#endif
