#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"


int main(int argc, char **argv) {
   init_file_system(argv[1],3);
   set_file_system_name(argv[1]);
   create_file(argv[2]);
   create_file(argv[3]);
   printf("\n");
   print_all_file();
   copy_file("test1");
   printf("\n");
   print_all_file();
   rename_file("copy","test3");
   printf("\n");
   print_all_file();
//   delete_file(argv[2]);
 //  print_all_file();

}


