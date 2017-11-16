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
    size_t nbytes = sizeof(new_file_block);
    size_t cbytes = sizeof(file_catalog);
    new_file_block result;
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = creat(file_name, mode);
    if(fd == -1)
        return UNKNOWN_ERROR;

    system_size = (int)ceil(system_size / VALUE_SIZE);
    double temp = (system_size * cbytes);
    int block_from_catalog = (int)ceil(temp / VALUE_SIZE);

    system_size += block_from_catalog;
    ftruncate(fd, (system_size ) * nbytes);

    if (fd < 0) {
        return UNKNOWN_ERROR;
    }

    for(int index = 0; index < system_size ; index++) {
        result.next = -1;
        result.number = index;
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
    new_file_block result;
    fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    int offset = 0;
    int is_find = 0;
    int is_free_file=0;
    block_count = file_size(fd) / nbytes;

    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    file_catalog catalog;
    int cbytes = sizeof(file_catalog);

    if(result.free_size == VALUE_SIZE) {
        memcpy(catalog.file_name, file_name, NAME_SIZE);
        catalog.is_free = 0;
        write_catalog_file(fd, &catalog, cbytes);
        int begin = search_free_block(fd);

        file_catalog* temp_buffer = (file_catalog*)malloc(sizeof(file_catalog));


        read_catalog_file(temp_buffer, fd, 0, sizeof(file_catalog));
        file_catalog str = *(temp_buffer);

        catalog.start = begin;
        change_catalog_file(fd, &catalog, 0, cbytes);

    } else {
        int catlog_size = 0;
        offset = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        int file_count = (int)catlog_size / cbytes;

        int pos = file_count * cbytes;
        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(file_header.is_free)
                is_free_file=1;
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free)
                return FILE_NAME_ERROR;
            if(file_header.is_free)
                pos = cbytes * i;
        }

        memcpy(catalog.file_name, file_name, NAME_SIZE);
        catalog.is_free = 0;
        int temp_sz=cbytes;
        if(is_free_file)
            temp_sz=0;
        int sz = get_free_file_system_size(fd);
        if(sz < temp_sz + VALUE_SIZE)
            return NOT_ENOUGH_MEMORY;
        file_catalog file_header;
        if(!is_free_file){
            write_catalog_file(fd, &catalog, cbytes);
            int begin = search_free_block(fd);


            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);

            read_catalog_file(temp_buffer, fd, pos, cbytes);
            file_header = *(temp_buffer);

            file_header.start = begin;
            if(is_free_file)
                file_header.is_free = 0;

        }
        else{
            int begin = search_free_block(fd);
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);

            read_catalog_file(temp_buffer, fd, pos, cbytes);
            file_header= *(temp_buffer);

            file_header.start = begin;
            file_header.is_free = 0;
            memcpy(file_header.file_name, file_name, NAME_SIZE);
        }
        change_catalog_file(fd, &file_header, pos, cbytes);

    }

    return SUCCESSFUL_CODE;
}

int write_catalog_file(int fd, void* value, int write_size)
{
    size_t nbytes;
    int offset = 0;
    new_file_block result;
    int is_find = 0;
    int start_pos = 0;
    int size = get_free_file_system_size(fd);

    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    block_count = file_size(fd) / nbytes;

    int prev = -1;
    int index = 0;

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
                        if(result.free_size == VALUE_SIZE) {
                            if(prev == -1) {
                                result.next = -1;
                            } else {
                                new_file_block prev_block;
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

                if(sz < result.free_size) {

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
    return SUCCESSFUL_CODE;
}


int change_catalog_file(int fd, void* buffer, int start, int count)
{
    int buffer_size = count;
    int sz = 0;
    if(start < 0 || count <= 0 ) {
        return INCORRECT_PARAMETERS_ERROR;
    }
    size_t nbytes;
    int is_find = 0;
    new_file_block result;
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    block_count = file_size(fd) / nbytes;
    int offset = 0;
    int temp_count = 0;
    int counter = 0;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if((temp_count <= start && start < temp_count + VALUE_SIZE) || counter) {
            if((temp_count <= start && start < temp_count + VALUE_SIZE)) {
                int size = 0;

                size = VALUE_SIZE - start % VALUE_SIZE;
                memcpy(result.value + start % VALUE_SIZE, buffer + sz, size);
                sz += size;
                count -= size;
            } else {
                int size = 0;
                if(count > VALUE_SIZE)
                    size = VALUE_SIZE;
                else
                    size = count;
                memcpy(result.value, buffer + sz, size);

                sz += size;
                count -= size;
            }
            lseek(fd, offset, SEEK_SET);
            write(fd, &result, nbytes);

            counter = 1;

            if(count <= 0)
                break;
        }
        temp_count += VALUE_SIZE;


        if(result.next == -1)
            break;
        else
            offset = result.next * nbytes;

    }

    return SUCCESSFUL_CODE;

}


int read_catalog_file(void* buffer, int fd, int start, int count)
{
    unsigned char* data = (unsigned char *)malloc(count);
    int buffer_size = count;
    int sz = 0;
    if(start < 0 || count <= 0 ) {
        return INCORRECT_PARAMETERS_ERROR;
    }
    size_t nbytes;
    int is_find = 0;
    new_file_block result;
    fd = open(file_system_name, O_RDONLY);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    block_count = file_size(fd) / nbytes;
    int offset = 0;
    int temp_count = 0;
    int counter = 0;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if((temp_count <= start && start < temp_count + VALUE_SIZE) || counter) {
            if((temp_count <= start && start < temp_count + VALUE_SIZE)) {
                int size = 0;
                if(count > VALUE_SIZE)
                    size = VALUE_SIZE - start % VALUE_SIZE;
                else
                    size = count;
                unsigned char* temp = (unsigned char*)malloc(size);
                memcpy(temp, result.value + (start - temp_count), size);
                memcpy(data + sz, temp, size);
                sz += size;
                count -= size;
            } else {
                int size = 0;
                if(count > VALUE_SIZE)
                    size = VALUE_SIZE;
                else
                    size = count;
                unsigned char* temp = (unsigned char*)malloc(size);
                memcpy(temp, result.value, size);
                memcpy(data + sz, temp, size);
                sz += size;
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

    }
    memcpy(buffer, data, buffer_size);
    return SUCCESSFUL_CODE;

    return FILE_NAME_ERROR;
}

int delete_file(char* file_name)
{
    int fd;
    size_t nbytes;
    new_file_block result;
    fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    int offset = 0;
    int is_find = 0;
    block_count = file_size(fd) / nbytes;

    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    file_catalog catalog;
    int cbytes = sizeof(file_catalog);

    if(result.free_size == VALUE_SIZE) {
        return FILE_NAME_ERROR;
    } else {
        int catlog_size = 0;
        offset = 0;
        int pos = 0;
        int find = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        int file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free) {
                find = 1;
                pos = cbytes * i;
            }
        }
        if(find) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);


            read_catalog_file(temp_buffer, fd, pos, cbytes);
            file_catalog file_header = *(temp_buffer);

            file_header.is_free = 1;
            change_catalog_file(fd, &file_header, pos, cbytes);
            offset = file_header.start * nbytes;

            for(int i = 0; i < block_count; i++) {
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);
                result.free_size = VALUE_SIZE;
                if(result.next == -1)
                    break;
                offset = result.next * nbytes;
            }
        } else
            return FILE_NAME_ERROR;


    }

    return SUCCESSFUL_CODE;
}

int write_file(char* file_name, void* value, int write_size)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    new_file_block result;
    int is_find = 0;
    int start_pos = 0;
    int size = 0;

    fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    block_count = file_size(fd) / nbytes;

    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    file_catalog catalog;
    int cbytes = sizeof(file_catalog);

    if(result.free_size == VALUE_SIZE) {
        return FILE_NOT_FOUND;
    } else {
        int catlog_size = 0;
        offset = 0;
        int pos = 0;
        int find = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        int file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free) {
                find = 1;
                start_pos = file_header.start;
            }
        }
        if(find) {
            offset = start_pos * nbytes;

            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);

            int fr_size = result.free_size;
            size = get_free_file_system_size(fd) + fr_size;
            if(write_size > size) {
                close(fd);
                return NOT_ENOUGH_MEMORY;
            }

            int prev = -1;
            int index = 0;
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
                                if(result.free_size == VALUE_SIZE) {
                                    if(prev == -1) {
                                        result.next = -1;
                                    } else {
                                        new_file_block prev_block;
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

                        if(sz < result.free_size) {

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
            return FILE_NAME_ERROR;
        }
        close(fd);
        return SUCCESSFUL_CODE;
    }
}

int read_file(void* buffer, char* file_name, int start, int count)
{
    unsigned char* data = (unsigned char *)malloc(count);
    int buffer_size = count;
    int sz = 0;
    if(start < 0 || count <= 0 ) {
        return INCORRECT_PARAMETERS_ERROR;
    }
    int fd;
    size_t nbytes;
    int is_find = 0;
    new_file_block result;
    fd = open(file_system_name, O_RDONLY);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    int cbytes = sizeof(file_catalog);
    block_count = file_size(fd) / nbytes;
    int offset = 0;
    int temp_count = 0;
    int counter = 0;


    if(result.free_size == VALUE_SIZE) {
        return FILE_NAME_ERROR;
    } else {
        int catlog_size = 0;
        offset = 0;
        int pos = 0;
        int find = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        int file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free) {
                find = 1;
                pos = cbytes * i;
            }
        }
        if(find) {
            for(int index = 0; index < block_count; index++) {
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);

                if((temp_count <= start && start < temp_count + VALUE_SIZE) || counter) {
                    if((temp_count <= start && start < temp_count + VALUE_SIZE)) {
                        int size = 0;
                        if(count > VALUE_SIZE)
                            size = VALUE_SIZE - start % VALUE_SIZE;
                        else
                            size = count;
                        unsigned char* temp = (unsigned char*)malloc(size);
                        memcpy(temp, result.value + (start - temp_count), size);
                        memcpy(data + sz, temp, size);
                        sz += size;
                        count -= size;
                    } else {
                        int size = 0;
                        if(count > VALUE_SIZE)
                            size = VALUE_SIZE;
                        else
                            size = count;
                        unsigned char* temp = (unsigned char*)malloc(size);
                        memcpy(temp, result.value, size);
                        memcpy(data + sz, temp, size);
                        sz += size;
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

            }

            close(fd);
            memcpy(buffer, data, buffer_size);
            return SUCCESSFUL_CODE;
        } else {
            close(fd);
            return FILE_NAME_ERROR;

        }

    }

    return FILE_NAME_ERROR;
}

int rename_file(char* file_name, char* new_name)
{
    int fd;
    size_t nbytes;
    new_file_block result;
    fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    int offset = 0;
    int is_find = 0;
    block_count = file_size(fd) / nbytes;

    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    file_catalog catalog;
    int cbytes = sizeof(file_catalog);

    if(result.free_size == VALUE_SIZE) {
        return FILE_NOT_FOUND;
    } else {
        int catlog_size = 0;
        offset = 0;
        int pos = 0;
        int find = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        int file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free) {
                find = 1;
                pos = cbytes * i;
            }
        }
        if(find) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);


            read_catalog_file(temp_buffer, fd, pos, cbytes);
            file_catalog file_header = *(temp_buffer);

            memcpy(file_header.file_name, new_name, NAME_SIZE);

            change_catalog_file(fd, &file_header, pos, cbytes);
            offset = file_header.start * nbytes;

        } else
            return FILE_NOT_FOUND;


    }

    return SUCCESSFUL_CODE;
}

int copy_file(char* file_name)
{
    int fd;
    size_t nbytes;
    int offset = 0;
    new_file_block result;
    int is_find = 0;
    int start_pos = 0;
    int size = 0;
    int is_free_file=0;
    int copy_size=0;
    int file_count;
    int pos = 0;

    fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;

    nbytes = sizeof(new_file_block);
    block_count = file_size(fd) / nbytes;

    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    file_catalog catalog;
    int cbytes = sizeof(file_catalog);

    if(result.free_size == VALUE_SIZE) {
        return FILE_NOT_FOUND;
    } else {
        int catlog_size = 0;
        offset = 0;

        int find = 0;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(file_header.is_free){
                is_free_file=1;
                pos=cbytes * i;
            }
            if(!strcmp(file_header.file_name, file_name) && !file_header.is_free) {
                is_find = 1;
                start_pos = file_header.start;
                break;
            }
        }
    }
    int index = 0;

    if(is_find) {
        offset = start_pos*nbytes;
        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            copy_size += VALUE_SIZE-result.free_size;
            if(result.next == -1)
                break;
        }
        int free_size = get_free_file_system_size(fd);
        char* copy_name = search_copy_name(file_name, fd,file_count);
        offset = start_pos*nbytes;
        int prev = -1;
        if(free_size > copy_size+cbytes) {
            file_catalog file_header;
            if(!is_free_file){
                write_catalog_file(fd, &file_header, cbytes);
                int begin = search_free_block(fd);


                file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
                pos=file_count*cbytes;
                read_catalog_file(temp_buffer, fd, pos, cbytes);
                file_header = *(temp_buffer);

                file_header.start = begin;
                memcpy(file_header.file_name,copy_name,NAME_SIZE);


            }
            else{
                int begin = search_free_block(fd);
                file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);

                read_catalog_file(temp_buffer, fd, file_count*cbytes, cbytes);
                file_header= *(temp_buffer);

                file_header.start = begin;
                file_header.is_free = 0;
                memcpy(file_header.file_name, copy_name, NAME_SIZE);

            }
            change_catalog_file(fd, &file_header, pos, cbytes);

            for(int i = 0; i < block_count; i++) {
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);
                for(; index < block_count; index++) {
                    new_file_block block;
                    lseek(fd, index * nbytes, SEEK_SET);
                    read(fd, &block, nbytes);

                    if(block.free_size==VALUE_SIZE) {
                        memcpy(block.value, result.value, VALUE_SIZE - result.free_size );
                        block.free_size = result.free_size;
                        if(prev == -1) {
                            block.next = -1;
                        } else {
                            new_file_block pr_block;
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


char* search_copy_name(char* file_name, int fd,int file_count)
{
    char copy_name[NAME_SIZE];

    file_catalog result;
    int cbytes = sizeof(file_catalog);
    int nbytes = sizeof(new_file_block);
    int block_count = file_size(fd) / nbytes;
    for(int index = 0; index < file_count; index++) {
        int is_find = 0;
        sprintf(copy_name, "%s(%d)", file_name, (index + 1));
        for(int id = 0; id < file_count; id++) {
            lseek(fd, id * cbytes, SEEK_SET);
            read(fd, &result, cbytes);

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

    size_t nbytes;
    int is_find = 0;
    nbytes = sizeof(new_file_block);
    int cbytes = sizeof(file_catalog);
    block_count = file_size(fd) / nbytes;
    new_file_block result;
    int block_count = file_size(fd) / nbytes;
    char file_name[NAME_SIZE][block_count];


    int offset = 0;
    int temp_count = 0;
    int counter = 0;
    int catlog_size=0;
    int file_count=0;
    int fl_count=0;


    if(result.free_size != VALUE_SIZE){

        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;
        }
        file_count = (int)catlog_size / cbytes;


        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!file_header.is_free){
                memcpy(file_name[fl_count],file_header.file_name,NAME_SIZE);
                fl_count++;
            }


        }
    }
    char **res=(char **)malloc(fl_count*sizeof(char*));
    for(int i = 0; i < fl_count; i++)
        res[i] = (char *)malloc(NAME_SIZE);

    for(int i = 0; i < fl_count; ++i)
            memcpy(res[i], file_name[i], NAME_SIZE);
    close(fd);
    return res;
}

int get_files_count(){
   int fd = open(file_system_name, O_RDWR);
    if(fd == -1)
        return UNKNOWN_ERROR;
    size_t nbytes;
    int is_find = 0;
    nbytes = sizeof(new_file_block);
    int cbytes = sizeof(file_catalog);
    block_count = file_size(fd) / nbytes;
    new_file_block result;
    int block_count = file_size(fd) / nbytes;
    char file_name[NAME_SIZE][block_count];


    int offset = 0;
    int temp_count = 0;
    int counter = 0;
    int catlog_size=0;
    int file_count=0;
    int count=0;
    lseek(fd, offset, SEEK_SET);
    read(fd, &result, nbytes);

    if(result.free_size == VALUE_SIZE) {
        return FILE_NAME_ERROR;
    } else {

        for(int i = 0; i < block_count; i++) {
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);
            offset = result.next * nbytes;
            catlog_size += VALUE_SIZE;
            if(result.next == -1)
                break;

        }
        file_count = (int)catlog_size / cbytes;

        for(int i = 0; i < file_count; i++) {
            file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
            read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
            file_catalog file_header = *(temp_buffer);
            if(!file_header.is_free)
                count++;

        }

    }
    close(fd);
    return count;
}

int file_size(int fd)
{
    struct stat s;
    if (fstat(fd, &s) == -1) {
        return -1 ;
    }
    return (s.st_size);
}

int search_free_block(int fd)
{
    size_t nbytes;
    new_file_block result;
    nbytes = sizeof(new_file_block);
    int offset = 0;
    int block_count = file_size(fd) / nbytes;
    int index;
    for(index = 0; index < block_count; index++) {
        lseek(fd, index * nbytes, SEEK_SET);
        read(fd, &result, nbytes);
        if(result.free_size == VALUE_SIZE)
            return index;
    }
    return -1;
}

int get_free_file_system_size(int fd)
{
    size_t nbytes, cbytes;
    int offset = 0;
    new_file_block result;
    int free_size = 0;
    nbytes = sizeof(new_file_block);
    cbytes = sizeof(file_catalog);

    block_count = file_size(fd) / nbytes;

    int catlog_size = 0;

    for(int i = 0; i < block_count; i++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        offset = result.next * nbytes;
        catlog_size += VALUE_SIZE;
        if(result.next == -1)
            break;
    }
    int file_count = (int)catlog_size / cbytes;

    offset = 0;
    int find = 0;
    for(int index = 0; index < block_count; index++) {
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(result.free_size == VALUE_SIZE) {
            for(int i = 0; i < file_count; i++) {
                file_catalog* temp_buffer = (file_catalog*)malloc(cbytes);
                read_catalog_file(temp_buffer, fd, cbytes * i, cbytes);
                file_catalog file_header = *(temp_buffer);
                if(result.number == file_header.start && !file_header.is_free)
                    find = 1;
            }
            if(!find)
                free_size += VALUE_SIZE;
            find = 0;
        }
        offset += nbytes;
    }
    return free_size;
}

int set_file_system_name(char* file_name)
{
    int fd = open(file_name, O_RDONLY);

    int nbytes = sizeof(new_file_block);
    if(fd==-1 || (file_size(fd) % nbytes))
        return INCORRECT_FILE_SYSTEM_NAME;
    close(fd);
    file_system_name = file_name;
    return SUCCESSFUL_CODE;
}

int print_all_block()
{
    int fd = open(file_system_name, O_RDWR);
    size_t nbytes, cbytes;
    int offset = 0;
    new_file_block result;
    int free_size = 0;
    nbytes = sizeof(new_file_block);
    cbytes = sizeof(file_catalog);

    block_count = file_size(fd) / nbytes;

    int catlog_size = 0;

    for(int i = 0; i < block_count; i++) {
        lseek(fd, i * nbytes, SEEK_SET);
        read(fd, &result, nbytes);
        printf("%s %i %i %i\n", result.value, result.number, result.next, result.free_size);
    }
    close(fd);

}

