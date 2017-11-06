#include <assert.h>
#include <malloc.h>
#include "../file_system.h"
#include "read_tests.h"

void read_file_name_error_test()
{
    init_file_system("file_system", 20);
    int buffer_size=10;

    char* temp_buffer = (char *) malloc(buffer_size);
    int err=read_file(temp_buffer,"test1",0,buffer_size);
    assert(err == -3);
}

void read_incorrect_parameters_error_test()
{
    init_file_system("file_system", 20);
    create_file("test1");
    int buffer_size=10;

    char* temp_buffer = (char *) malloc(buffer_size);
    int err=read_file(temp_buffer,"test1",0,-32);
    assert(err == -1);
}

void read_begin_correct_test()
{
    int err;
    init_file_system("file_system", 50);
    char* text="b";
    char* resuit="b";
    create_file("test1");
    err=write_file("test1",text,strlen(text));
    int buffer_size=1;

    char* temp_buffer = (char *) malloc(buffer_size);
    err=read_file(temp_buffer,"test1",0,buffer_size);
    assert(err == 0);
    assert(memcmp(resuit,temp_buffer,buffer_size)==0);

}

void read_center_correct_test()
{
    int err;
    init_file_system("file_system", 50);
    char* text="big string to copy, very big and long... @Alex ";
    char* resuit="tring to c";
    create_file("test1");
    err=write_file("test1",text,strlen(text));
    int buffer_size=10;

    char* temp_buffer = (char *) malloc(buffer_size);
    err=read_file(temp_buffer,"test1",5,buffer_size);
    assert(err == 0);
    assert(memcmp(resuit,temp_buffer,buffer_size)==0);
}

void read_end_correct_test()
{
    int err;
    init_file_system("file_system", 50);
    char* text="big string to copy, very big and long... @Alex ";
    char* resuit="Alex ";
    create_file("test1");
    err=write_file("test1",text,strlen(text));
    int buffer_size=5;

    char* temp_buffer = (char *) malloc(10);
    err=read_file(temp_buffer,"test1",42,buffer_size);
    assert(err == 0);
    assert(memcmp(resuit,temp_buffer,5)==0);
}

void read_not_char_test()
{
    int err;
    int buffer_size=sizeof(int);
    init_file_system("file_system", 16);
    int number=13;
    int* test= (int *) malloc(buffer_size);
    test=&number;
    create_file("test1");
    err=write_file("test1","testtt",strlen("testtt"));
    err=write_file("test1",test,sizeof(int));
    assert(err == 0);

    int* temp_buffer = (int *) malloc(buffer_size);
    err=read_file(temp_buffer,"test1",6,sizeof(test));
    assert(err == 0);
    assert(number==*temp_buffer);
}

void read_tests()
{
    read_file_name_error_test();
    read_incorrect_parameters_error_test();
    read_begin_correct_test();
    read_center_correct_test();
    read_end_correct_test();
    read_not_char_test();

}
