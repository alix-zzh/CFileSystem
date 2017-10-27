#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>     /* uint32_t */
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/mman.h>
#include "file_system.h"

const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

int read_all_file(char* name,char* extension) {
   int fd;
   size_t nbytes;
   file_block result;
    fd = open(file_system_name, O_RDONLY);
    nbytes = sizeof(file_block);

    int offset=0;
    while(1){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        printf( "%s ",result.value);
        if(result.next==-1)
            break;
        else
            offset=result.next*nbytes;
    }
   close(fd);




   return EXIT_SUCCESS;
}

int write_file(char* name,char* extension,char* value) {
   int fd;
   size_t nbytes;
   file_block result;
    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);

    int offset=0;
    while(1){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        printf( "%s ",result.value);
        if(result.next==-1)
            break;
        else
            offset=result.next*nbytes;
    }

   close(fd);




   return EXIT_SUCCESS;
}

int create_file(char* name,char* extension) {
    int fd;
    size_t nbytes;
    file_block result;
    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);
    int offset=0;
    int is_find=0;
    block_count=file_size(fd)/nbytes;
    for(int index=0;index<block_count;index++){
       lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(result.is_free==1){
            is_find=1;
            break;
        }
        offset+=nbytes;
    }
    if(is_find){
        sprintf(result.name,name);
        sprintf(result.extension,extension);
        result.is_free=0;
        lseek(fd, offset, SEEK_SET);
        write(fd, &result, nbytes);
    }
    close(fd);

    return EXIT_SUCCESS;
}

int delete_file(char* name,char* extension){
    int fd;
    size_t nbytes;
    int offset=0;
    file_block result;

    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);

    while(1){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.name,name)||strcmp(result.extension,extension))){
            result.is_free=1;
            lseek(fd, offset, SEEK_SET);
            write(fd, &result, nbytes);
            if(result.next==-1)
                break;
            else
                offset=result.next*nbytes;
        }
        else
            offset+=nbytes;
    }
    close(fd);
    return EXIT_SUCCESS;
}

int init_file_system(char* name,uint32_t size){
    block_count=size-1;
    int fd;
   size_t nbytes;
   file_block result;
    fd = creat(name, mode);
    ftruncate(fd, 3*sizeof(file_block));

   if (fd < 0) {
      fprintf(stderr, "Unable to open  %s\n",strerror(errno));
      exit(EXIT_FAILURE);
   }

   nbytes = sizeof(file_block);
    for(int index=0;index<size;index++){
        result.number = index;
        result.next = -1;
        result.is_free= 1;
        result.is_start= 1;
        result.free_size=sizeof(result.value);

    write(fd, &result, nbytes);
    }
    close(fd);

    return 0;
}

int file_size(int fd) {
   struct stat s;
   if (fstat(fd, &s) == -1) {
      int saveErrno = errno;
      fprintf(stderr, "fstat(%d) returned errno=%d.", fd, saveErrno);
      return(-1);
   }
   return(s.st_size);
}

void set_file_system_name(char* name){
    file_system_name=name;
}

void print_all_file(){
    int fd;
    size_t nbytes;
    file_block result;
    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);

    for(int index=0;index<block_count;index++){
        lseek(fd, index*nbytes, SEEK_SET);
        read(fd, &result, nbytes);
        if(!result.is_free)
            printf("%s.%s\n",result.name,result.extension);
    }
}


