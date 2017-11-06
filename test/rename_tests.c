#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "../file_system.h"
#include "rename_tests.h"

void rename_file_not_find_test()
{
    init_file_system("file_system", 20);
    int err=rename_file("test1","new_file");
    assert(err == -1);
}

void rename_currect_name_test()
{
    init_file_system("file_system", 20);
    create_file("test1");
    write_file("test1", "big test",strlen("big test"));
    int err=rename_file("test1","new_file");
    assert(err == 0);
    err=create_file("new_file");
    assert(err == -3);
}

void rename_value_test()
{
    int err;
    init_file_system("file_system", 50);
    char* text="big string to copy, very big and long... @Alex ";
    create_file("test1");
    write_file("test1",text,strlen(text));
    int buffer_size=47;
    err=rename_file("test1","new_file");

    char* temp_buffer = (char *) malloc(buffer_size);
    err=read_file(temp_buffer,"new_file",0,buffer_size);
    assert(memcmp(text,temp_buffer,buffer_size)==0);
}


void rename_tests()
{
    rename_file_not_find_test();
    rename_currect_name_test();
    rename_value_test();
}
