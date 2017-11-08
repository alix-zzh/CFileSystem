#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"
#include "test/init_tests.h"
#include "test/create_tests.h"
#include "test/delete_tests.h"
#include "test/write_tests.h"
#include "test/read_tests.h"
#include "test/copy_tests.h"
#include "test/rename_tests.h"
#include <time.h>


int main(int argc, char **argv)
{
    init_tests();
    create_tests();
    delete_tests();
    write_tests();
    read_tests();
    rename_tests();
    copy_tests();
    printf("Test complete\n");

    /*FILE *fp;
    fp = fopen("res_static.txt", "w+");
    init_file_system("file_system", 400);
    char* text="big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex";
    create_file("test1");
    write_file("test1",text,strlen(text));


    for(int i=0;i<strlen(text);i++){
        char* temp_buffer = (char *) malloc(1);
        unsigned long start = clock();

        read_file(temp_buffer,"test1",i,1);


        unsigned long time_spent = (unsigned long)(clock() - start) ;
        printf("%s",temp_buffer);
        fprintf(fp, "%i \n",time_spent);
        free(temp_buffer);

    }
*/
    command_line();
}


