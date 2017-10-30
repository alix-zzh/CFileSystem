#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"


int main(int argc, char **argv) {
    char test[10];
    char* big_string ="big string to copy, very big and long... @Alex ";

   init_file_system(argv[1],9);
   set_file_system_name(argv[1]);
   create_file("test1");
   create_file("test2");

   write_file("test1",big_string);
   write_file("test2","Write to test2");
   //read_all_file("test1");
   //delete_file("test1");
   //create_file("test3");
   //create_file("test1");
   print_all_file();
   printf("\n--------\n");
   write_file("test1","APPEND");
   write_file("test1","<APPEND>");
   print_all_file();
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


