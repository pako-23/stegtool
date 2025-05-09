#include "container.h"
#include <assert.h>
#include <stdlib.h>


int main(void)
{
    struct container *c = init();

    assert(c != NULL);

    assert(insert(c, 10) != 0);
    assert(insert(c, 20) != 0);
    assert(insert(c, 20) == 0);

    assert(find(c, 20) != 0);
    assert(find(c, 21) == 0);

    assert(remove(c, 1) == 0);
    assert(remove(c, 10) != 0);
    assert(remove(c, 20) != 0);
    assert(remove(c, 12) == 0);

    assert(insert(c, 10) != 0);
    assert(remove(c, 10) != 0);
    assert(insert(c, 10) != 0);

    destroy(c);

    return EXIT_SUCCESS;
}
