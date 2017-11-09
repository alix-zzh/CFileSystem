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
#include <string.h>
#include <time.h>

#define INIT 436
#define TOUCH 547
#define RM 223
#define COPY 443
#define MV 227
#define WRITE 555
#define READ 412
#define DIR 319
#define HELP 425
#define SET 332
#define EXIT 442

void tests(){
    init_tests();
    create_tests();
    delete_tests();
    write_tests();
    read_tests();
    rename_tests();
    copy_tests();
    printf("Test complete\n");
}

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

int command_line()
{
    char* command = (char*)malloc(COMMAND_SIZE * sizeof(char));
    int file_system_set = 0;
    int exit = 1;
    while(exit) {
        printf("file_system:~$ ");
        scanf("%s", command);
        int code = 0;
        for(int index = 0; index < strlen(command); index++) {
            code += command[index];
        }

        int err;
        int amount_of_files = 0;
        char* file_name = (char*)malloc(NAME_SIZE * sizeof(char));
        switch(code) {
        case INIT:
            scanf("%s", file_name);
            int system_size;
            scanf("%d", &system_size);
            err = init_file_system(file_name, system_size);

            if(err == INCORRECT_PARAMETERS_ERROR) {
                printf("Wrong parameters\n");
            }
            if(err == UNKNOWN_ERROR) {
                printf("Something wrong\n");
            }
            if(err == SUCCESSFUL_CODE) {
                file_system_set = 1;
            }
            break;
        case SET:
            scanf("%s", file_name);
            err = set_file_system_name(file_name);

            if(err == INCORRECT_FILE_SYSTEM_NAME) {
                printf("Incorrect file system name\n");
            }
            if(err == SUCCESSFUL_CODE) {
                file_system_set = 1;
            }
            break;
        case RM:
            scanf("%s", file_name);
            if(file_system_set) {
                err = delete_file(file_name);

                if(err == FILE_NAME_ERROR) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            break;
        case COPY:
            scanf("%s", file_name);
            if(file_system_set) {
                err = copy_file(file_name);

                if(err == NOT_ENOUGH_MEMORY) {
                    printf("Memory is not enough to copy file\n");
                }
                if(err == FILE_NAME_ERROR) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            break;
        case MV:
            scanf("%s", file_name);
            char* new_file_name = (char*)malloc(NAME_SIZE * sizeof(char*));
            scanf("%s", new_file_name);
            if(file_system_set) {
                err = rename_file(file_name, new_file_name);

                if(err == FILE_NOT_FOUND) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            break;
        case WRITE:
            scanf("%s", file_name);
            char* value = (char*)malloc(MAX_WRITE_READ_VALUE * sizeof(char));
            fgets(value, MAX_WRITE_READ_VALUE, stdin);
            value++;
            if(file_system_set) {
                err = write_file(file_name, value, strlen(value));

                if(err == NOT_ENOUGH_MEMORY) {
                    printf("Memory is not enough to write in\n");
                }
                if(err == FILE_NOT_FOUND) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            break;
        case READ:
            scanf("%s", file_name);
            int start;
            scanf("%d", &start);
            int count;
            scanf("%d", &count);
            char* read_value = (char*)malloc(MAX_WRITE_READ_VALUE * sizeof(char));
            if(file_system_set) {
                err = read_file(read_value, file_name, start, count);

                if(err == INCORRECT_PARAMETERS_ERROR) {
                    printf("Wrong parameters\n");
                }
                if(err == FILE_NAME_ERROR) {
                    printf("No such file\n");
                }
                if(err == SUCCESSFUL_CODE) {
                    printf("%s\n", read_value);
                }
            } else
                printf("Set or init file system\n");
            break;
        case TOUCH:
            scanf("%s", file_name);
            if(file_system_set) {
                err = create_file(file_name);

                if(err == FILE_NAME_ERROR) {
                    printf("Error with name of file\n");
                }
                if(err == NOT_ENOUGH_MEMORY) {
                    printf("Memory is not enough to create file\n");
                }
            } else
                printf("Set or init file system\n");
            break;
        case DIR:
            if(file_system_set) {
                amount_of_files = get_files_count();
                char** file_names = get_files_name();
                for(int index = 0; index < amount_of_files; index++) {
                    printf("%s\n", file_names[index]);
                }
            } else
                printf("Set or init file system\n");
            break;
        case HELP:
            printf("init: name, size\nset: name\ntouch: name\nrm: name\ncopy: name\nmv: name\nwrite: name, value\nread: name, begin, amount\ndir\n");
            break;
        case EXIT:
            exit = 0;
            printf("Exit\n");
            break;
        default:
            printf("No such command  \n");

        }
    }

    return SUCCESSFUL_CODE;
}

int main(int argc, char **argv)
{

    tests();
    load_test();
    command_line();
}
