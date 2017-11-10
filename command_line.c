#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"
#include "command_line.h"
#include <string.h>


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
            break;
        default:
            printf("No such command  \n");

        }
    }

    return SUCCESSFUL_CODE;
}
