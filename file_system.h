#ifndef FILE_SYSTEM_H_INCLUDED
#define FILE_SYSTEM_H_INCLUDED



#define NAME_SIZE 10
#define EXTENSION 5
#define VALUE_SIZE 10

int block_count;
char* file_system_name;

typedef struct block {
   char name[NAME_SIZE];
   char extension[EXTENSION];
   int number;
   int next;
   char value[VALUE_SIZE];
   int is_free;
   int is_start;
   int size;
   int free_size;
} file_block;

#endif // FILE_SYSTEM_H_INCLUDED
