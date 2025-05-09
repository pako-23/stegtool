#include "container.h"
/**
 * Include assert.h header to use the assert function. The assert
 * function takes an expression. If the expression is false, it
 * terminates the program reporting a failure.
 */
#include <assert.h>
#include <stdlib.h>


int main(void)
{
    assert(init() != 0);

    assert(insert(10) != 0);
    assert(insert(20) != 0);
    assert(insert(20) == 0);

    assert(find(20) != 0);
    assert(find(21) == 0);

    assert(remove(1) == 0);
    assert(remove(10) != 0);
    assert(remove(20) != 0);
    assert(remove(12) == 0);

    assert(insert(10) != 0);
    assert(remove(10) != 0);
    assert(insert(10) != 0);

    destroy();

    return EXIT_SUCCESS;
}
