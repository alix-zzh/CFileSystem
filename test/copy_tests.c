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
    init_file_system("file_system", 10);
    create_file("test1");
    write_file("test1", "big test",strlen("big test"));
    int err=copy_file("test1");
    assert(err == -2);
}

void copy_currect_name_test()
{
    init_file_system("file_system", 20);
    create_file("test1");
    write_file("test1", "big test",strlen("big test"));
    int err=copy_file("test1");
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

    char* temp_buffer = (char *) malloc(buffer_size);
    read_file(temp_buffer,"test1(1)",0,buffer_size);
    assert(memcmp(&text,&temp_buffer,buffer_size));
}


void copy_tests()
{
    copy_file_not_find_test();
    copy_not_enough_memory_test();
    copy_currect_name_test();
    copy_value_test();
}
