#include <assert.h>
#include "../file_system.h"
#include "create_tests.h"

void write_file_not_find_test()
{
    init_file_system("file_system", 20);
    int err = write_file("test6", "test");
    assert(err == -1);

}

void write_not_enough_memory_test()
{
    init_file_system("file_system", 10);
    int err = create_file("test1");
    err = write_file("test1", "big test to write ...");
    assert(err == -2);
}

void write_to_start_test()
{
    init_file_system("file_system", 20);
    int err = create_file("test1");
    err = write_file("test1", "test");
    assert(err == 0);
}

void write_append_test()
{
    init_file_system("file_system", 40);
    int err = create_file("test1");
    err = write_file("test1", "test");
    assert(err == 0);
    err = write_file("test1", "append text and smth ...");
    assert(err == 0);
}

void write_tests()
{
    write_file_not_find_test();
    write_not_enough_memory_test();
    write_to_start_test();
    write_append_test();
}
