#ifndef FILE_SYSTEM_H_INCLUDED
#define FILE_SYSTEM_H_INCLUDED

#include <stdint.h>

#define SUCCESSFUL_CODE  0;
#define INCORRECT_PARAMETERS_ERROR -1;
#define NOT_ENOUGH_MEMORY -2;
#define UNKNOWN_ERROR 1;
#define FILE_NOT_FOUND -1;

#define NAME_SIZE 10
#define VALUE_SIZE 10

int     block_count;
char* file_system_name;

typedef struct block {
   char name[NAME_SIZE];
   int  number;
   int  next;
   char value[VALUE_SIZE];
   int  is_free;
   int  is_start;
   int  free_size;
} file_block;

int     create_file(char* name);
int     delete_file(char* name);
int     init_file_system(char* name,int system_size);
int     file_size(int fd);
void    set_file_system_name(char* name);
void    print_all_file();
char*   search_copy_name(char* name,int fd);
int     copy_file(char* name);
char*   read_file(char* name,int start,int count);
int     write_file(char* name,char* value);
int     get_free_file_system_size(int fd);

#endif // FILE_SYSTEM_H_INCLUDED
