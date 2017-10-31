#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"


int main(int argc, char **argv) {

    char* big_string ="big string to copy, very big and long... @Alex and Anton";
    char* little_string="Write to test2";

    init_file_system(argv[1],200);
    set_file_system_name(argv[1]);
    create_file(argv[2]);
    create_file(argv[3]);

    write_file(argv[2],big_string);
    write_file(argv[3],little_string);
    copy_file(argv[2]);
    copy_file(argv[2]);
    print_all_file();
    printf("%s\n",read_file("test1(2)",2,20));


}


