#include <stdio.h>
#include <stdlib.h>
#include "../file_system.h"
#include <string.h>
#include <time.h>

void load_test(){

    FILE *fp;
    fp = fopen("res_static.txt", "w");
    init_file_system("file_system", 400);
    char* text="big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex";
    create_file("test1");
    write_file("test1",text,strlen(text));


    for(int i=0;i<strlen(text);i++){
        char* temp_buffer = (char *) malloc(1);
        unsigned long start = clock();

        read_file(temp_buffer,"test1",i,1);


        unsigned long time_spent = (unsigned long)(clock() - start) ;
        fprintf(fp, "%lu \n",time_spent);
        free(temp_buffer);

    }
    fclose(fp);
}
