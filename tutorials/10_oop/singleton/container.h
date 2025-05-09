#ifndef CONTAINER_H_INCLUDED
#define CONTAINER_H_INCLUDED

/**
 * Initializes a container structure as a singleton returning 0 on
 * failure, and a non-zero value on success.
 */
int init(void);

/**
 * Destroys a container singleton releasing all allocated resources.
 */
void destroy(void);

/**
 * Inserts value into the container returing 0 on failure or if value
 * is already in the container, and a non-zero value on success.
 */
int insert(int value);

/**
 * Removes value from the container returning 0 on failure or if the
 * element does not exist, and a non-zero value on success.
 */
int remove(int value);

/**
 * Returns 0 if value is not present into the container, and a
 * non-zero if value is present.
 */
int find(int value);

#endif
