#include "container.h"
#include <stdlib.h>


/* Define node of a linked list */
struct node {
    int value;
    struct node *next;
};


int init(struct container *c)
{
    c->head = NULL;
    return 1;
}

void destroy(struct container *c)
{
    struct node *tmp;

    while (c->head) {
	tmp = c->head;
	c->head = c->head->next;
	free(tmp);
    }
    /**
     * We do not own the container, it is responsibility of the caller
     * to deallocate it.
     */
}

int insert(struct container *c, int value)
{
    struct node *node;

    if (find(c, value) != 0) return 0;

    if ((node = malloc(sizeof(struct node))) == NULL)
	return 0;

    node->value = value;
    node->next = c->head;
    c->head = node;

    return 1;
}

int remove(struct container *c, int value)
{
    struct node *p, *tmp;

    if (c->head == NULL) return 0;

    if (c->head->value == value) {
        tmp = c->head;
	c->head = c->head->next;
	free(tmp);
	return 1;
    }

    for (p = c->head; p->next; p = p->next) {
	if (p->next->value == value) {
	    tmp = p->next;
	    p->next = p->next->next;
	    free(tmp);
	    return 1;
	}
    }

    return 0;
}

int find(struct container *c, int value)
{
    struct node *p;

    for (p = c->head; p; p = p->next)
	if (p->value == value)
	    return 1;

    return 0;
}
