#include <assert.h>
#include "../file_system.h"
#include "init_tests.h"

void init_incorrect_peremeters_error_test()
{
    int err = init_file_system("file_system", 0);
    assert(err == -1);
}

void init_correct_test()
{
    int err = init_file_system("file_system", 100);
    assert(err == 0);
}

void init_tests()
{
    init_incorrect_peremeters_error_test();
    init_correct_test();
}
