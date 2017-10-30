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

const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

int init_file_system(char* name,int system_size){
    int fd;
   size_t nbytes;
   file_block result;
    fd = creat(name, mode);
    ftruncate(fd, system_size*sizeof(file_block));

   if (fd < 0) {
      fprintf(stderr, "Unable to open  %s\n",strerror(errno));
      exit(EXIT_FAILURE);
   }

   nbytes = sizeof(file_block);
    for(int index=0;index<system_size;index++){
        result.number = index;
        result.next = -1;
        result.is_free= 1;
        result.is_start= 1;
        result.free_size=sizeof(result.value);

    write(fd, &result, nbytes);
    }
    close(fd);

    return SUCCESSFUL_CODE;
}

int create_file(char* name) {
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
        memcpy(result.name,name,strlen(name)+1);
        result.is_free=0;
        lseek(fd, offset, SEEK_SET);
        write(fd, &result, nbytes);
    }
    close(fd);

    return SUCCESSFUL_CODE;
}

int delete_file(char* name){
    int fd;
    size_t nbytes;
    int offset=0;
    file_block result;

    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);

    while(1){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.name,name))){
            result.is_free=1;
            result.free_size=VALUE_SIZE;
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
    return SUCCESSFUL_CODE;
}

char* read_file(char* name,int start,int count) {
    char* res = malloc(count);
   int fd;
   size_t nbytes;
   file_block result;
    fd = open(file_system_name, O_RDONLY);
    nbytes = sizeof(file_block);

    int offset=0;
    int temp_count=0;
    int counter=0;
    for(int index=0;index<block_count;index++){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(!(strcmp(result.name,name)) ){
            if((temp_count<=start && start<temp_count+VALUE_SIZE) || counter){
                if((temp_count<=start && start<temp_count+VALUE_SIZE)){
                    int size=0;
                    if(count>VALUE_SIZE)
                        size=VALUE_SIZE-start%VALUE_SIZE;
                    else
                        size=count;
                    char* temp= malloc(size);
                    memcpy(temp,result.value+(start-temp_count),size);
                    strcat(res, temp);
                    count-=size;
                }
                else{
                    int size=0;
                    if(count>VALUE_SIZE)
                        size=VALUE_SIZE;
                    else
                        size=count;
                    char* temp= malloc(size);
                    memcpy(temp,result.value,size);
                    strcat(res, temp);
                    count-=size;
                }

                counter=1;

                if(count<=0)
                    break;
            }
            temp_count+=VALUE_SIZE;

            if(result.next==-1)
               break;
            else
                offset=result.next*nbytes;
        }
        else
            offset+=nbytes;

    }
   close(fd);




   return res;
}

int write_file(char* name,char* value) {
    int fd;
    size_t nbytes;
    int offset=0;
    file_block result;
    int is_find=0;
    int start_pos=0;

    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);
    block_count=file_size(fd)/nbytes;
    if(strlen(value)>get_free_file_system_size(fd)){
        printf("Not enough memory");
        return -2;
    }
    for(int index=0;index<block_count;index++){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.name,name)) && result.is_start){
            start_pos=result.number;
            is_find=1;
            break;
        }
        offset+=nbytes;
    }
    offset=start_pos*nbytes;
    int prev=-1;
    int index=0;
    if(is_find){
        for(int index=0;index<block_count;index++){
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);

            if(result.next!=-1)
                offset=result.next*nbytes;
            else{
                int free_size=result.free_size;
                for(int char_offset=0;char_offset<(strlen(value));char_offset+=free_size){
                    prev=result.number;
                    if(result.free_size==0){
                        for(;index<block_count;index++){
                            lseek(fd, index*nbytes, SEEK_SET);
                            read(fd, &result, nbytes);

                            if(result.is_free){
                                result.is_free=0;
                                result.is_start=0;
                                memcpy(result.name, name, sizeof(result.name));
                                if(prev==-1){
                                   result.next=-1;
                                }
                                else{
                                    file_block prev_block;
                                    lseek(fd, prev*nbytes, SEEK_SET);
                                    read(fd, &prev_block, nbytes);
                                    prev_block.next=result.number;
                                    lseek(fd, prev*nbytes, SEEK_SET);
                                    write(fd,&prev_block,nbytes);

                                }


                                offset=index*nbytes;
                                break;
                            }

                        }
                    }



                    memcpy(result.value+(sizeof(result.value)-result.free_size),value+char_offset,result.free_size);
                    int sz=strlen(value)-char_offset;

                    if(sz<result.free_size)
                        result.free_size=VALUE_SIZE-sz;
                    else{
                        result.free_size=0;
                    }
                    lseek(fd, offset, SEEK_SET);
                    write(fd, &result, nbytes);

                }
                break;

            }

        }
    }
    else{
        printf("File not found %s",name);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int copy_file(char* name){
    int fd;
    size_t nbytes;
    int offset=0;
    file_block result;
    int start_pos=0;
    int copy_size=0;
    int is_find=0;

    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);
    block_count=file_size(fd)/nbytes;

    for(int index=0;index<block_count;index++){
       lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if(!(strcmp(result.name,name)) && result.is_start){
            start_pos=result.number;
            is_find=1;
            for(int id=0;id<block_count;id++){
               lseek(fd, start_pos, SEEK_SET);
                read(fd, &result, nbytes);
                if(result.next!=-1){
                    copy_size+=result.number;
                    offset=result.next*nbytes;
                }
                else
                    break;

            }
            break;
        }
        offset+=nbytes;
    }
    int index=0;
    if(is_find){
        int free_size=get_free_file_system_size(fd);
        char* copy_name=search_copy_name(name,fd);
        offset=start_pos;
        int prev=-1;
        if(free_size>copy_size){
            for(int i=0;i<block_count;i++){
                int free_offset=0;
                lseek(fd, offset, SEEK_SET);
                read(fd, &result, nbytes);
                for(;index<block_count;index++){
                    file_block block;
                    lseek(fd, index*nbytes, SEEK_SET);
                    read(fd, &block, nbytes);

                    if(block.is_free){
                        memcpy(block.name, copy_name, sizeof(block.name));
                        memcpy(block.value, result.value, sizeof(result.value)-result.free_size+1);
                        block.free_size=result.free_size;
                        block.is_free=0;
                        block.is_start=result.is_start;
                        if(prev==-1){
                           block.next=-1;
                        }
                        else{
                            file_block prev_block;
                            lseek(fd, prev*nbytes, SEEK_SET);
                            read(fd, &prev_block, nbytes);
                            prev_block.next=block.number;
                            lseek(fd, prev*nbytes, SEEK_SET);
                            write(fd, &prev_block, nbytes);
                        }
                        prev=block.number;


                        lseek(fd, index*nbytes, SEEK_SET);
                        write(fd,&block,nbytes);
                        break;
                    }

                }
                if(result.next!=-1)
                    offset=result.next*nbytes;
                else
                    break;
            }
        }
        else
            printf("Not memory to copy %s",name);
            close(fd);
            return -2;
    }
    else{
        printf("File not found %s",name);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int rename_file(char* name,char* new_name){
    int fd;
    size_t nbytes;
    int offset=0;
    file_block result;
    int is_find=0;
    int start_pos=0;

    fd = open(file_system_name, O_RDWR);
    nbytes = sizeof(file_block);
    block_count=file_size(fd)/nbytes;

    for(int index=0;index<block_count;index++){
        lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);

        if(!(strcmp(result.name,name)) && result.is_start){
            start_pos=result.number;
            is_find=1;
            break;
        }
        offset+=nbytes;
    }
    offset=start_pos*nbytes;
    if(is_find){
        for(int index=0;index<block_count;index++){
            lseek(fd, offset, SEEK_SET);
            read(fd, &result, nbytes);

            memcpy(result.name, new_name, sizeof(result.name));

            lseek(fd, offset, SEEK_SET);
            write(fd, &result, nbytes);
            if(result.next!=-1)
                offset=result.next*nbytes;
            else
                break;
        }
    }
    else{
        printf("File not found %s",name);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int get_free_file_system_size(int fd){
    size_t nbytes;
    int offset=0;
    file_block result;
    int free_size=0;
    nbytes = sizeof(file_block);

    block_count=file_size(fd)/nbytes;

    for(int index=0;index<block_count;index++){
       lseek(fd, offset, SEEK_SET);
        read(fd, &result, nbytes);
        if( result.is_free){
            free_size+=sizeof(result.value);
        }
        offset+=nbytes;
    }
    return free_size;
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
            print_block(result);
    }
}

void print_block(file_block block){
    printf("\n name: %s \n index: %i \n next: %i \n value: %s \n free : %i\n ",block.name,block.number,block.next,block.value,block.free_size);
}

char* search_copy_name(char* name,int fd){
    char copy_name[NAME_SIZE];

    file_block result;
    int nbytes = sizeof(file_block);
    int block_count=file_size(fd)/nbytes;
    for(int index=0;index<block_count;index++){
        int is_find=0;
        sprintf(copy_name, "%s(%d)",name,(index+1));
        for(int id=0;id<block_count;id++){
            lseek(fd, id*nbytes, SEEK_SET);
            read(fd, &result, nbytes);

            if(!(strcmp(result.name,copy_name)))
                is_find=1;

        }
        if(!is_find)
            break;


    }
    char *res = malloc(NAME_SIZE);
    for(int i = 0; i < NAME_SIZE; ++i)
        res[i] = copy_name[i];

    printf("%s\n",copy_name);
    return res;
}
