#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"


int main(int argc, char **argv) {
   init_file_system("1.txt",3);
   // read_all_file("test1","txt");
   set_file_system_name("1.txt");
   create_file("test1","txt");
   create_file("test2","sql");
   print_all_file();
}


