#ifndef CONTAINER_H_INCLUDED
#define CONTAINER_H_INCLUDED

struct node;

struct container {
    struct node *head;
};


/**
 * Initializes a container object. It takes a pointer to an allocated
 * container, and initializes its internal members.  It returns 0 on
 * failure, and non-zero on success.
 */
int init(struct container *c);

/**
 * Destroys a container object releasing all allocated resources.
 */
void destroy(struct container *c);

/**
 * Inserts value into the container returing 0 on failure or if value
 * is already in the container, and a non-zero value on success.
 */
int insert(struct container *c, int value);

/**
 * Removes value from the container returning 0 on failure or if the
 * element does not exist, and a non-zero value on success.
 */
int remove(struct container *c, int value);

/**
 * Returns 0 if value is not present into the container, and a
 * non-zero if value is present.
 */
int find(struct container *c, int value);

#endif
