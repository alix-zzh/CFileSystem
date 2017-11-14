#include <stdio.h>

#include "file_system.h"
#include "test/init_tests.h"
#include "test/create_tests.h"
#include "test/delete_tests.h"
#include "test/write_tests.h"
#include "test/read_tests.h"
#include "test/copy_tests.h"
#include "test/rename_tests.h"
#include "test/load_test.h"
#include "command_line.h"



void tests(){
    init_tests();
    create_tests();
    delete_tests();
    write_tests();
    read_tests();
    rename_tests();
   // copy_tests();
    printf("Test complete\n");
}

int main(int argc, char **argv)
{



    tests();
    //load_test();
    //command_line();
}
