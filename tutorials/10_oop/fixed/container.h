#ifndef CONTAINER_H_INCLUDED
#define CONTAINER_H_INCLUDED

struct container;

/**
 * Creates a new container object. It returns a valid pointer on
 * success; otherwise, it returns a NULL pointer.
 */
struct container *init(void);

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
