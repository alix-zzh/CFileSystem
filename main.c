#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"
#include "test/init_tests.h"
#include "test/create_tests.h"
#include "test/delete_tests.h"
#include "test/write_tests.h"
#include "test/read_tests.h"
#include <time.h>


int main(int argc, char **argv)
{
    init_tests();
    create_tests();
    delete_tests();
    write_tests();
    read_tests();
    copy_tests();

/*
    char* big_string ="Abcdefghiklmnop";
    init_file_system(argv[1],4*10);
    create_file("test1");
    for(int i=0;i<2;i++)
    write_file("test1",big_string);

*/
    int buffer_size=00;

    char* temp_buffer = (char *) malloc(0);

  //  printf("%i\n",read_file(temp_buffer,"test1",6,20));
   // printf("%s",temp_buffer);
/*
    FILE *fp;
    fp = fopen("res_static.txt", "w+");

    for(int i=0;i<23*10;i++){

        unsigned long start = clock();

        read_file("test1",i,1);

        unsigned long time_spent = (unsigned long)(clock() - start) ;

        fprintf(fp, "%i \n",time_spent);

    }

/*

//    char* ddd ="Big string to copy, very big and long... @Alex and Anton";

            char* big_string ="A123456789A123456789A123456789A";
            char* little_string="Write to test2";

            init_file_system(argv[1],200);
            create_file(argv[2]);
            create_file(argv[3]);

            write_file(argv[2],big_string);
            write_file(argv[3],little_string);
            copy_file(argv[2]);
            copy_file(argv[2]);
            print_all_file();
            printf("%s\n",read_file("test1(2)",0,20));

*/
}


