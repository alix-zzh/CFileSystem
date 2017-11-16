#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "../file_system.h"
#include "copy_tests.h"

void copy_file_not_find_test()
{
    init_file_system("file_system", 20);
    int err=copy_file("test1");
    assert(err == -1);
}

void copy_not_enough_memory_test()
{
    init_file_system("file_system", 8);
    create_file("test1");
    write_file("test1", "big test",strlen("big test"));
    int err=copy_file("test1");
    assert(err == -2);
}

void copy_currect_name_test()
{
    init_file_system("file_system", 64);
    create_file("test1");
    write_file("test1", "big test",strlen("big test"));
    int err=copy_file("test1");
    char** file_names = get_files_name();

    assert(err == 0);
    err=create_file("test1(1)");
    assert(err == -3);
}

void copy_value_test()
{
    init_file_system("file_system", 100);
    char* text="big string to copy, very big and long... @Alex ";
    create_file("test1");
    write_file("test1",text,strlen(text));
    int buffer_size=47;
    copy_file("test1");

    char* temp_buffer = (char *) malloc(strlen(text));
    read_file(temp_buffer,"test1(1)",0,strlen(text));
    assert(memcmp(text,temp_buffer,strlen(text))==0);
}

void copy_full_test()
{
    init_file_system("file_system", 16);
    char* text="big stri";
    create_file("test1");
    write_file("test1",text,strlen(text));
    int buffer_size=47;
    int err=copy_file("test1");

    char* temp_buffer = (char *) malloc(strlen(text));
    read_file(temp_buffer,"test1(1)",0,strlen(text));
    assert(memcmp(text,temp_buffer,strlen(text))==0);
}

void copy_tests()
{
    copy_file_not_find_test();
    copy_not_enough_memory_test();
    copy_currect_name_test();
    copy_value_test();
    copy_full_test();
}
