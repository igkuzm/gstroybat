/**
 * File              : cp.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "cp.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int k_lib_cp(const char *from, const char *to)
{
	char ch;
   FILE *source, *target;
 
   source = fopen(from, "r");
   target = fopen(to, "w");

   while( ( ch = fgetc(source) ) != EOF )
      fputc(ch, target);
 
   printf("File copied successfully.\n");
 
   fclose(source);
   fclose(target);
 
   return 0;
}
