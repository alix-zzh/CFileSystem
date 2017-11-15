#include <assert.h>
#include "../file_system.h"
#include "delete_tests.h"

void delete_correct_test()
{
    init_file_system("file_system", 20);
    int err = create_file("test1");
    err = delete_file("test1");
    assert(err == 0);
}

void delete_file_name_error_test()
{
    init_file_system("file_system", 20);
    int err = create_file("test1");
    assert(err == 0);
    err = delete_file("test3");
    assert(err == -3);
}

void delete_correct_free_size_test()
{
    init_file_system("file_system", 8);
    int err = create_file("test1");
    assert(err == 0);
    err = delete_file("test1");
    assert(err == 0);
    err = create_file("test1");
    assert(err == 0);
}

void delete_tests()
{
    delete_correct_test();
    delete_correct_free_size_test();
    delete_file_name_error_test();
}
