#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"


int main(int argc, char **argv) {
    char test[10];
    char* big_string ="big string to copy, very big and long... @Alex";
    test[0]='A';
/*
    for(int char_offset=0;char_offset<(strlen(big_string));char_offset+=10){
        memcpy(test+1,big_string+char_offset,sizeof(test)-1);
        printf("%s \n",test);
    }
*/


   init_file_system(argv[1],9);
   set_file_system_name(argv[1]);
   create_file(argv[2]);
   create_file(argv[3]);
   printf("\n");
   print_all_file();
   write_file("test1",big_string);
   write_file("test2","lol29");
   read_all_file("test1");

 /*
   copy_file("test1");
   printf("\n");
   print_all_file();
   rename_file("copy","test3");
   printf("\n");
   print_all_file();
*/

//   delete_file(argv[2]);
 //  print_all_file();

}


