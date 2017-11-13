#ifndef FILE_SYSTEM_H_INCLUDED
#define FILE_SYSTEM_H_INCLUDED

#include <stdint.h>
#include <math.h>

#define SUCCESSFUL_CODE  0
#define INCORRECT_PARAMETERS_ERROR -1
#define NOT_ENOUGH_MEMORY -2
#define FILE_NAME_ERROR -3
#define UNKNOWN_ERROR 1
#define FILE_NOT_FOUND -1
#define INCORRECT_FILE_SYSTEM_NAME -1

#define MAX_WRITE_READ_VALUE 1024
#define NAME_SIZE 9
#define VALUE_SIZE 8

int     block_count;
char*   file_system_name;
/*
typedef struct block {
    unsigned char file_name[NAME_SIZE];
    int  number;
    int  next;
    char value[VALUE_SIZE];
    int  is_free;
    int  is_start;
    int  free_size;
} file_block;
*/
typedef struct new_block {
    int  next;
    char value[VALUE_SIZE];
    int  free_size;
    int  number;
} new_file_block;

typedef struct catalog {
    char file_name[NAME_SIZE];
    int  start;
    int  is_free;
    int  free_size;
} file_catalog;

int     command_line();
int     init_file_system(char* file_name, int system_size);
int     create_file(char* file_name);
int     delete_file(char* file_name);
int     write_file(char* file_name, void* value,int write_size);
int     read_file(void* buffer,char* file_name, int start, int count);
int     copy_file(char* file_name);
int     rename_file(char* file_name, char* new_name);

char*   search_copy_name(char* file_name, int fd);
int     file_size(int fd);
int     get_free_file_system_size(int fd);
int    set_file_system_name(char* file_name);
char**  get_files_name();
int     get_files_count();

#endif // FILE_SYSTEM_H_INCLUDED
