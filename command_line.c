#include <stdio.h>
#include <stdlib.h>
#include "file_system.h"
#include "command_line.h"
#include <string.h>


int command_line()
{
    char* command = (char*)malloc(MAX_WRITE_READ_VALUE * sizeof(char));
    int file_system_set = 0;
    int exit = 1;
    while(exit) {
        printf("file_system:~$ ");
        scanf("%s", command);
        int code = 0;
        for(int index = 0; index < strlen(command); index++) {
            code += command[index]*(index+1)*COUNT;
        }

        int err;
        int amount_of_files = 0;
        char* file_name = (char*)malloc(NAME_SIZE * sizeof(char));
        switch(code) {
        case INIT:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
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
            free(file_name);
            break;
        case SET:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            err = set_file_system_name(file_name);

            if(err == INCORRECT_FILE_SYSTEM_NAME) {
                printf("Incorrect file system name\n");
            }
            if(err == SUCCESSFUL_CODE) {
                file_system_set = 1;
            }
            free(file_name);
            break;
        case CREATE:
            ;
            char* name_of_new_file = (char*)malloc(NAME_SIZE);
            err = read_string(name_of_new_file);
            name_of_new_file[strlen(name_of_new_file)-1] = 0;
            memmove(name_of_new_file, name_of_new_file+1, strlen(name_of_new_file));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            if(file_system_set) {
                err = create_file(name_of_new_file);
                if(err == FILE_NAME_ERROR) {
                    printf("Error with name of file\n");
                }
                if(err == NOT_ENOUGH_MEMORY) {
                    printf("Memory is not enough to create file\n");
                }
            } else
                printf("Set or init file system\n");
            free(name_of_new_file);
            break;
        case RM:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            if(file_system_set) {
                err = delete_file(file_name);

                if(err == FILE_NAME_ERROR) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            free(file_name);
            break;
        case COPY:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
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
            free(file_name);
            break;
        case MV:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            char* new_file_name = (char*)malloc(NAME_SIZE * sizeof(char*));
            err = read_string(new_file_name);
            new_file_name[strlen(new_file_name)-1] = 0;
            memmove(new_file_name, new_file_name+1, strlen(new_file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            if(file_system_set) {
                err = rename_file(file_name, new_file_name);

                if(err == FILE_NOT_FOUND) {
                    printf("No such file\n");
                }
            } else
                printf("Set or init file system\n");
            free(file_name);
            free(new_file_name);
            break;
        case WRITE:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
            char* value = (char*)malloc(MAX_WRITE_READ_VALUE * sizeof(char));
            err = read_string(value);
            value[strlen(value)-1] = 0;
            memmove(value, value+1, strlen(value));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
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
            free(file_name);
            free(value);
            break;
        case READ:
            err = read_string(file_name);
            file_name[strlen(file_name)-1] = 0;
            memmove(file_name, file_name+1, strlen(file_name));
            if(err == WRONG_PARAMETERS){
                printf("Wrong parameters\n");
                break;
            }
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
            free(file_name);
            free(read_value);
            break;
        case LS:
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
            printf("init: name, size\nset_system: name\ncreate: name\nrm: name\ncopy: name\nmv: name\nwrite: name, value\nread: name, begin, amount\nls\n");
            break;
        case EXIT:
            exit = 0;
            unmmaped();
            break;
        default:
            printf("No such command %i %s\n",code,command);

        }
    }
    free(command);

    return SUCCESSFUL_CODE;
}

int read_string(char* name){
    char* temp = (char*)malloc(NAME_SIZE);
    while(1){
        scanf("%s", temp);
        if(strlen(name)!=0){
            strcat(name, " ");
        }
        strcat(name, temp);
        if(name[0] != '"'){
            return WRONG_PARAMETERS;
        }
        if(name[strlen(name)-1] == '"'){
            break;
        }
    }

    return 0;
}
