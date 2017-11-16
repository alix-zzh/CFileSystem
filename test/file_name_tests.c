#include <assert.h>
#include <string.h>
#include "../file_system.h"
#include "file_name_tests.h"

void get_file_names_correct_test()
{
    init_file_system("file_system", 24);
    int err = create_file("test1");
    assert(err == 0);
    err = create_file("test2");
    assert(err == 0);
    err = create_file("test3");
    assert(err == 0);
    int amount_of_files = get_files_count();
    assert(amount_of_files == 3);
    char** file_names = get_files_name();
    assert(memcmp(file_names[0],"test1",strlen("test1")) == 0);
    assert(memcmp(file_names[1],"test2",strlen("test2")) == 0);
    assert(memcmp(file_names[2],"test3",strlen("test3")) == 0);
}

void get_file_names_delete_correct_test()
{
    init_file_system("file_system", 24);
    int err = create_file("test1");
    assert(err == 0);
    err = create_file("test2");
    assert(err == 0);
    err = create_file("test3");
    assert(err == 0);
    err=delete_file("test1");
    err=delete_file("test3");
    int amount_of_files = get_files_count();
    assert(amount_of_files == 1);
    char** file_names = get_files_name();
    assert(memcmp(file_names[0],"test2",NAME_SIZE) == 0);

}

void file_names_tests()
{
    get_file_names_correct_test();
    get_file_names_delete_correct_test();
}
