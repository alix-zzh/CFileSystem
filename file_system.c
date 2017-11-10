#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/mman.h>
#include "file_system.h"

int init_file_system(char* file_name, int system_size)
{
    if(system_size <= 0) {
        return INCORRECT_PARAMETERS_ERROR;
    }
    int fd;
    size_t nbytes = sizeof(file_block);;
    file_block result;
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = creat(file_name, mode);
    if(fd==-1)
        return UNKNOWN_ERROR;

    system_size = (int)ceil(system_size / VALUE_SIZE);
    ftruncate(fd, system_size * nbytes);

    if (fd < 0) {
        return UNKNOWN_ERROR;
    }


    for(int index = 0; index < system_size; index++) {
        result.number = index;
        result.next = -1;
        result.is_free = 1;
        result.is_start = 1;
        result.free_size = VALUE_SIZE;

        write(fd, &result, nbytes);
    }
    close(fd);
    file_system_name = file_name;
    return SUCCESSFUL_CODE;
}

int create_file(char* file_name)
{
    int fd;
    size_t nbytes;
    file_block result;
    fd = open(file_system_name, O_RDWR);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    int offset = 0;
    int is_find = 0;
    block_count = file_size(fd) / nbytes;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(!(strcmp(result.file_name, file_name) || result.is_free)) {
            return FILE_NAME_ERROR;
        }
        offset += nbytes;
    }
    offset = 0;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(result.is_free == 1) {
            is_find = 1;
            break;
        }
        offset += nbytes;
    }
    if(is_find) {
        memcpy(result.file_name, file_name, NAME_SIZE);
        result.is_free = 0;
        lseek(fd, offset, SEEK_SET);
        write(fd, &result, nbytes);
    } else
        return NOT_ENOUGH_MEMORY;
    close(fd);

    return SUCCESSFUL_CODE;
}

int delete_file(char* file_name)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    file_block result;

    fd = open(file_system_name, O_RDWR);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    int is_find = 0;

    while(1) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!strcmp(result.file_name, file_name) && !result.is_free) {
            is_find = 1;
            result.is_free = 1;
            result.free_size = VALUE_SIZE;
            lseek(fd, offset, SEEK_SET);
            write(fd, &result, nbytes);
            if(result.next == -1)
                break;
            else
                offset = result.next * nbytes;
        } else
            offset += nbytes;
    }
    close(fd);

    if(!is_find)
        return FILE_NAME_ERROR;

    return SUCCESSFUL_CODE;
}

int read_file(void* buffer,char* file_name, int start, int count)
{
    if(start < 0 || count <= 0 ) {
        return INCORRECT_PARAMETERS_ERROR;
    }
    int fd;
    size_t nbytes;
    int is_find=0;
    file_block result;
    fd = open(file_system_name, O_RDONLY);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    block_count = file_size(fd) / nbytes;
    int offset = 0;
    int temp_count = 0;
    int counter = 0;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(!(strcmp(result.file_name, file_name)) && !result.is_free) {
            is_find=1;
            if((temp_count <= start && start < temp_count + VALUE_SIZE) || counter) {
                if((temp_count <= start && start < temp_count + VALUE_SIZE)) {
                    int size = 0;
                    if(count > VALUE_SIZE)
                        size = VALUE_SIZE - start % VALUE_SIZE;
                    else
                        size = count;
                    char* temp = malloc(size);
                    memcpy(temp, result.value + (start - temp_count), size);
                    strcat(buffer, temp);
                    count -= size;
                } else {
                    int size = 0;
                    if(count > VALUE_SIZE)
                        size = VALUE_SIZE;
                    else
                        size = count;
                    char* temp = malloc(size);
                    memcpy(temp, result.value, size);
                    strcat(buffer, temp);
                    count -= size;
                }

                counter = 1;

                if(count <= 0)
                    break;
            }
            temp_count += VALUE_SIZE;

            if(result.next == -1)
                break;
            else
                offset = result.next * nbytes;
        } else
            offset += nbytes;

    }

    close(fd);
    if(is_find)
        return SUCCESSFUL_CODE;
    return FILE_NAME_ERROR;
}

int write_file(char* file_name, void* value,int write_size)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    file_block result;
    int is_find = 0;
    int start_pos = 0;
    int size = 0;

    fd = open(file_system_name, O_RDWR);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    block_count = file_size(fd) / nbytes;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.file_name, file_name)) && result.is_start && !result.is_free) {
            size = get_free_file_system_size(fd) + result.free_size;
            start_pos = result.number;
            is_find = 1;
            break;
        }
        offset += nbytes;
    }

    offset = start_pos * nbytes;
    int prev = -1;
    int index = 0;
    if(is_find) {
        if(write_size > size) {
            close(fd);
            return NOT_ENOUGH_MEMORY;
        }
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);

            if(result.next != -1)
                offset = result.next * nbytes;
            else {
                for(int char_offset = 0; char_offset < write_size; ) {
                    prev = result.number;
                    if(result.free_size == 0) {
                        for(; index < block_count; index++) {
                            lseek(fd, index * nbytes, SEEK_SET);
                            read(fd, &result, nbytes);
                            if(result.is_free) {
                                result.is_free = 0;
                                result.is_start = 0;
                                memcpy(result.file_name, file_name, sizeof(result.file_name));
                                if(prev == -1) {
                                    result.next = -1;
                                } else {
                                    file_block prev_block;
                                    lseek(fd, prev * nbytes, SEEK_SET);
                                    read(fd, &prev_block, nbytes);
                                    prev_block.next = result.number;
                                    lseek(fd, prev * nbytes, SEEK_SET);
                                    write(fd, &prev_block, nbytes);

                                }


                                offset = index * nbytes;
                                break;
                            }

                        }
                    }

                    memcpy(result.value + (VALUE_SIZE - result.free_size), value + char_offset, result.free_size);

                    int sz = write_size - char_offset;

                    if(sz < result.free_size){

                        result.free_size = VALUE_SIZE - sz;
                        char_offset += result.free_size;
                        lseek(fd, offset, SEEK_SET);
                        write(fd, &result, nbytes);
                        break;
                    }
                    char_offset += result.free_size;
                    result.free_size = 0;

                    lseek(fd, offset, SEEK_SET);
                    write(fd, &result, nbytes);


                }
                break;

            }

        }
    } else {
        close(fd);
        return FILE_NOT_FOUND;
    }
    close(fd);
    return SUCCESSFUL_CODE;
}

int copy_file(char* file_name)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    file_block result;
    int start_pos = 0;
    int copy_size = 0;
    int is_find = 0;

    fd = open(file_system_name, O_RDWR);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    block_count = file_size(fd) / nbytes;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(!(strcmp(result.file_name, file_name)) && result.is_start) {
            start_pos = result.number;
            is_find = 1;
            for(int id = 0; id < block_count; id++) {
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);
                if(result.next != -1) {
                    copy_size += VALUE_SIZE;
                    offset = result.next * nbytes;
                } else
                    break;

            }
            break;
        }
        offset += nbytes;
    }
    int index = 0;
    if(is_find) {
        int free_size = get_free_file_system_size(fd);
        char* copy_name = search_copy_name(file_name, fd);
        offset = start_pos;
        int prev = -1;
        if(free_size > copy_size) {
            for(int i = 0; i < block_count; i++) {
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);
                for(; index < block_count; index++) {
                    file_block block;
                    lseek(fd, index * nbytes, SEEK_SET);
                    read(fd, &block, nbytes);

                    if(block.is_free) {
                        memcpy(block.file_name, copy_name, sizeof(block.file_name));
                        memcpy(block.value, result.value, VALUE_SIZE - result.free_size );
                        block.free_size = result.free_size;
                        block.is_free = 0;
                        block.is_start = result.is_start;
                        if(prev == -1) {
                            block.next = -1;
                        } else {
                            file_block pr_block;
                            lseek(fd, prev * nbytes, SEEK_SET);
                            read(fd, &pr_block, nbytes);

                            pr_block.next=block.number;

                            lseek(fd, prev * nbytes, SEEK_SET);
                            write(fd, &pr_block, nbytes);

                        }
                        prev = block.number;


                        lseek(fd, index * nbytes, SEEK_SET);
                        write(fd, &block, nbytes);
                        break;
                    }

                }
                if(result.next != -1)
                    offset = result.next * nbytes;
                else
                    break;
            }
        } else{
            close(fd);
            return NOT_ENOUGH_MEMORY;
        }

    } else {
        close(fd);
        return FILE_NOT_FOUND;
    }
    close(fd);
    return SUCCESSFUL_CODE;
}

int rename_file(char* file_name, char* new_name)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    file_block result;
    int is_find = 0;
    int start_pos = 0;

    fd = open(file_system_name, O_RDWR);
    if(fd==-1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(file_block);
    block_count = file_size(fd) / nbytes;

    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.file_name, file_name)) && result.is_start && !result.is_free) {
            start_pos = result.number;
            is_find = 1;
            break;
        }
        offset += nbytes;
    }
    offset = start_pos * nbytes;
    if(is_find) {
        for(int index = 0; index < block_count; index++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);

            memcpy(result.file_name, new_name, sizeof(result.file_name));

            lseek(fd, offset, SEEK_SET);
            write(fd, &result, nbytes);
            if(result.next != -1)
                offset = result.next * nbytes;
            else
                break;
        }
    } else {
        close(fd);
        return FILE_NOT_FOUND;
    }
    close(fd);
    return SUCCESSFUL_CODE;
}

int get_free_file_system_size(int fd)
{
    size_t nbytes;
    int offset = 0;
    file_block result;
    int free_size = 0;
    nbytes = sizeof(file_block);

    block_count = file_size(fd) / nbytes;

    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(result.is_free) {
            free_size += sizeof(result.value);
        }
        offset += nbytes;
    }
    return free_size;
}

int file_size(int fd)
{
    struct stat s;
    if (fstat(fd, &s) == -1) {
        return -1 ;
    }
    return (s.st_size);
}

int set_file_system_name(char* file_name)
{
    int fd = open(file_name, O_RDONLY);

    int nbytes = sizeof(file_block);
    if(fd==-1 || (file_size(fd) % nbytes))
        return INCORRECT_FILE_SYSTEM_NAME;
    close(fd);
    file_system_name = file_name;
    return SUCCESSFUL_CODE;
}

char* search_copy_name(char* file_name, int fd)
{
    char copy_name[NAME_SIZE];

    file_block result;
    int nbytes = sizeof(file_block);
    int block_count = file_size(fd) / nbytes;
    for(int index = 0; index < block_count; index++) {
        int is_find = 0;
        sprintf(copy_name, "%s(%d)", file_name, (index + 1));
        for(int id = 0; id < block_count; id++) {
            lseek(fd, id * nbytes, SEEK_SET);
            read(fd, &result, nbytes);

            if(!(strcmp(result.file_name, copy_name)))
                is_find = 1;

        }
        if(!is_find)
            break;


    }
    char *res = malloc(NAME_SIZE);
    for(int i = 0; i < NAME_SIZE; ++i)
        res[i] = copy_name[i];

    return res;
}

char** get_files_name(){
    int fd = open(file_system_name, O_RDWR);
    int nbytes = sizeof(file_block);
    int block_count = file_size(fd) / nbytes;
    char file_name[NAME_SIZE][block_count];

    file_block result;

    int id = 0;
    for(int index = 0; index < block_count; index++) {

        lseek(fd, index * nbytes, SEEK_SET);
        read(fd, &result, nbytes);

        if(result.is_start && !result.is_free){
            memcpy(file_name[id], result.file_name, NAME_SIZE);
            id++;
        }

    }
    char **res=(char **)malloc(block_count*sizeof(char*));
    for(int i = 0; i < block_count; i++)
        res[i] = (char *)malloc(NAME_SIZE);

    for(int i = 0; i < block_count; ++i)
            memcpy(res[i], file_name[i], NAME_SIZE);
    close(fd);
    return res;
}

int get_files_count(){
    int fd = open(file_system_name, O_RDWR);
    int nbytes = sizeof(file_block);
    int block_count = file_size(fd) / nbytes;

    file_block result;

    int count = 0;
    for(int index = 0; index < block_count; index++) {

        lseek(fd, index * nbytes, SEEK_SET);
        read(fd, &result, nbytes);

        if(result.is_start && !result.is_free){
            count++;
        }

    }
    close(fd);
    return count;
}
