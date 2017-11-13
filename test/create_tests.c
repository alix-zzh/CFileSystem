#include <assert.h>
#include "../file_system.h"
#include "create_tests.h"

void create_correct_test()
{
    init_file_system("file_system", 24);
    int err = create_file("test1");
    assert(err == 0);
    err = create_file("test2");
    assert(err == 0);

}

void create_not_enough_memory_test()
{
    init_file_system("file_system", 16);
    int err = create_file("test1");
    assert(err == 0);
    err = create_file("test2");
    assert(err == 0);
    err = create_file("test3");
    assert(err == -2);
}

void create_file_name_error_test()
{
    init_file_system("file_system", 20);
    int err = create_file("test1");
    assert(err == 0);
    err = create_file("test1");
    assert(err == -3);
}

void create_tests()
{
    create_correct_test();
    create_file_name_error_test();
    create_not_enough_memory_test();
}
