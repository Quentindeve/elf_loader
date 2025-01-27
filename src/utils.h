#ifndef UTILS_H
#define UTILS_H

// Asserts that var != NULL, exits otherwise
#define ASSERT_NOTNULL(var)                             \
    do                                                  \
    {                                                   \
        if (var == NULL)                                \
        {                                               \
            fprintf(stderr, "Error: out of memory.\n"); \
            exit(69);                                   \
        }                                               \
    } while (0)

#endif