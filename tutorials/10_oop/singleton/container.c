#include "container.h"
#include <stdlib.h>


/* Define node of a linked list */
struct node {
    int value;
    struct node *next;
};

/**
 * Define the head of the linked list as a global static variable. The
 * static keyword refers to the fact that this variable cannot be
 * accessed from outside of this source file.
 */
static struct node *head = NULL;

/* We simply return a non-zero value since the head is already NULL. */
int init(void) { return 1; }

void destroy(void)
{
    struct node *tmp;

    while (head) {
	tmp = head;
	head = head->next;
	free(tmp);
    }
}

int insert(int value)
{
    struct node *node;

    if (find(value) != 0) return 0;

    if ((node = malloc(sizeof(struct node))) == NULL)
	return 0;

    node->value = value;
    node->next = head;
    head = node;

    return 1;
}

int remove(int value)
{
    struct node *p, *tmp;

    if (head == NULL) return 0;

    if (head->value == value) {
        tmp = head;
	head = head->next;
	free(tmp);
	return 1;
    }

    for (p = head; p->next; p = p->next) {
	if (p->next->value == value) {
	    tmp = p->next;
	    p->next = p->next->next;
	    free(tmp);
	    return 1;
	}
    }

    return 0;
}

int find(int value)
{
    struct node *p;

    for (p = head; p; p = p->next)
	if (p->value == value)
	    return 1;

    return 0;
}
