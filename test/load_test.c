#include <stdio.h>
#include <stdlib.h>
#include "../file_system.h"
#include <string.h>
#include <time.h>
#include <math.h>

void load_test(){

    FILE *fp;
    fp = fopen("res_static.txt", "w");
    init_file_system("file_system", 500);
    char* text="big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex @Alex big string to copy, very big and long... @Alex big string to copy, very big and long... @Alex@Alex big string to copy, very big and long... @Alex@Alex big string to copy, very big and long... @Alex1234567890";
    create_file("test1");
    write_file("test1",text,strlen(text));


    for(int i=0;i<strlen(text);i++){
        char* temp_buffer = (char *) malloc(1);
        unsigned long start = clock();

        for(int j=0;j<100;j++)
            read_file(temp_buffer,"test1",i,1);


        unsigned long time_spent = (unsigned long)(clock() - start) ;
        fprintf(fp, "%lu \n",llround(((double)time_spent)/100));
        free(temp_buffer);

    }
    fclose(fp);
}
