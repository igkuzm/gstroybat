/**
 * File              : cp_recursive.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "cp_recursive.h"
#include "cp.h"
#include "ls.h"

#include <sys/stat.h>

struct k_lib_cp_recursive_args {
	const char *source;
	const char *dest;
	int depth;
};

int k_lib_cp_recursive_callback(KLibFile *file, void *user_data){
	struct k_lib_cp_recursive_args *args = user_data;
	
	char source[BUFSIZ];
	sprintf(source, "%s/%s", args->source, file->filename);
	
	char dest[BUFSIZ];
	sprintf(dest, "%s/%s", args->dest, file->filename);

#if defined _WIN32 || defined _WIN64
	sprintf(source, "%s\\%s", args->source, file->filename);
	sprintf(dest, "%s\\%s", args->dest, file->filename);
#endif
	
	if (file->filetype == KLIBFILETYPE_FILE)
		k_lib_cp(source, dest);

	if (file->filetype == KLIBFILETYPE_DIR){
#if defined _WIN32 || defined _WIN64		
		if (mkdir(dest) == -1) {
#else
		if (mkdir(dest, 0700) == -1) {
#endif
			perror("Can't create directory");
		} else {
			if (args->depth < 10 ) {
				struct k_lib_cp_recursive_args new_ags;
				new_ags.source = source;
				new_ags.dest = dest;
				new_ags.depth = args->depth + 1;			
			
				k_lib_ls(source, &new_ags, k_lib_cp_recursive_callback);
			}
		}
	}

	return 0;
}

int k_lib_cp_recursive(const char *from, const char *to) {
	struct k_lib_cp_recursive_args args;
	args.source = from;
	args.dest = to;
	args.depth = 0;
	return k_lib_ls(from, &args, k_lib_cp_recursive_callback);
}
