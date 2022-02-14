/**
 * File              : cp.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 11.10.2021
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include <stdio.h>
#include <stdbool.h>

//copy file FROM path to destination TO path
extern int cp(const char *from, const char *to, bool owerwrite);

extern int cp_recursive(const char *from, const char *to, bool owerwrite);

//check if file exists
extern bool file_exists (char *filename);
