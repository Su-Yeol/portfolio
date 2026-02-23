#ifndef PROFILE_CATALOG_H
#define PROFILE_CATALOG_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    const char *name;
    const char *category;
    const char *path;
} firmware_profile_t;

size_t profile_count(void);
const firmware_profile_t *profile_at(size_t index);
const firmware_profile_t *profile_find(const char *name);
int profile_show(const firmware_profile_t *profile, FILE *out);
int profile_validate_required(const firmware_profile_t *profile, char *missing, size_t missing_size);

#endif
