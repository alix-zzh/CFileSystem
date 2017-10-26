#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>     /* uint32_t */
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>

const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

typedef struct block {
   char* name;
   char* extension;
   uint32_t number;
   int next;
   void* value;
   int is_free;
} file_block;

#define FILENAME "temp/1"

/* 'Safe' write */
int safewrite( int fd, const void *p, size_t want) {
   int ret;

   errno = 0;
   while (want) {
      ret = write(fd, (uint8_t *)p, want);
      if (ret <= 0) {
         if (errno != EINTR && errno != EAGAIN) {
            return -1;
         }
         errno = 0;
         continue;
      }
      want -= ret;
      p = (uint8_t*) p + ret;
   }
   return 0;
}

int saferead(int fd, const void *p,size_t end) {
   int ret;

   errno = 0;
   while (end) {
      ret = read(fd, (uint8_t*)p, end);
      if( ret == 0 )
         return -1;  /* EOF */
      if (ret <= 0) {
         if( errno != EINTR && errno != EAGAIN ) {
            return -1;
         }
         errno = 0;
         continue;
      }
      end -= ret;
      p = (uint8_t*) p + ret;
   }
   return 0;
}


int main(int argc, char **argv) {
   int fd;
   file_block result;
   size_t nbytes;

   // init_file_system();

   //chmod("temp", mode_t mode);
 // if(fd = open(FILENAME, O_WRONLY)<0){
    fd = creat(FILENAME, mode);
    ftruncate(fd, 100);
  // }

   if (fd < 0) {
      fprintf(stderr, "Unable to open " FILENAME ": %s\n",
            strerror(errno));
      exit(EXIT_FAILURE);
   }
   nbytes = sizeof(file_block);

   result.name = "test1";
   result.extension = "txt";
   result.number = 0;
   result.value= "hello";
   result.next = 2;
   result.is_free= 0;


   if (0 != safewrite(fd, &result, nbytes)) {
      fprintf(stderr, "Unable to write to " FILENAME ": %s\n",
            strerror(errno));
      exit(EXIT_FAILURE);
   }

   result.name = "test2";
   result.extension = "txt";
   result.number = 1;
   result.value= "hello";
   result.next = 93;
   result.is_free= 1;

   if (0 != safewrite(fd, &result, nbytes)) {
      fprintf(stderr, "Unable to write to " FILENAME ": %s\n",
            strerror(errno));
      exit(EXIT_FAILURE);
   }

    result.name = "test1";
    result.extension = "txt";
    result.number = 2;
    result.value= " world";
    result.next = -1;
    result.is_free= 1;

    if (0 != safewrite(fd, &result, nbytes)) {
      fprintf(stderr, "Unable to write to " FILENAME ": %s\n",
            strerror(errno));
      exit(EXIT_FAILURE);
    }


    close(fd);
    fd = -1;

    fd = open(FILENAME, O_RDONLY);
    nbytes = sizeof(file_block);

    int offset=0;
    while(1){
        lseek(fd, offset, SEEK_SET);
        if (0 != saferead(fd, &result, nbytes)) {
            fprintf(stderr, "Unable to read file \"" FILENAME "\": %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf( "%s ",result.value);
        if(result.next==-1)
            break;
        else
            offset=result.next*nbytes;
    }

   close(fd);




   return EXIT_SUCCESS;
}


