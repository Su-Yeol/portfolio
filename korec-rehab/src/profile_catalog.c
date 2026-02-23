#include "profile_catalog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRED_COUNT 2
#define READ_BUFFER_SIZE 512

static const char *kRequiredTokens[REQUIRED_COUNT] = {
    "init_uart(",
    "stop_control(",
};

static const firmware_profile_t kProfiles[] = {
    {"profile_default_control", "profile_default", "src/firmware_profiles/profile_default/profile_default_control.c"},
    {"profile_default_diag_board_v2", "profile_default", "src/firmware_profiles/profile_default/profile_default_diag_board_v2.c"},
    {"profile_double_lock_control", "profile_double_lock", "src/firmware_profiles/profile_double_lock/profile_double_lock_control.c"},
    {"profile_double_lock_diag_board_v2", "profile_double_lock", "src/firmware_profiles/profile_double_lock/profile_double_lock_diag_board_v2.c"},
    {"profile_lock_mode_control", "profile_lock_mode", "src/firmware_profiles/profile_lock_mode/profile_lock_mode_control.c"},
    {"profile_lock_mode_diag_board_v2", "profile_lock_mode", "src/firmware_profiles/profile_lock_mode/profile_lock_mode_diag_board_v2.c"},
    {"profile_offset_control_baseline", "profile_offset", "src/firmware_profiles/profile_offset/profile_offset_control_baseline.c"},
    {"profile_offset_control_overlap", "profile_offset", "src/firmware_profiles/profile_offset/profile_offset_control_overlap.c"},
    {"profile_offset_diag_board_v2", "profile_offset", "src/firmware_profiles/profile_offset/profile_offset_diag_board_v2.c"},
    {"transport_serial_profile_v1", "transport_serial", "src/firmware_profiles/transport_serial/transport_serial_profile_v1.c"},
};

size_t profile_count(void) {
    return sizeof(kProfiles) / sizeof(kProfiles[0]);
}

const firmware_profile_t *profile_at(size_t index) {
    if (index >= profile_count()) {
        return NULL;
    }
    return &kProfiles[index];
}

const firmware_profile_t *profile_find(const char *name) {
    size_t i;

    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < profile_count(); ++i) {
        if (strcmp(kProfiles[i].name, name) == 0) {
            return &kProfiles[i];
        }
    }

    return NULL;
}

int profile_show(const firmware_profile_t *profile, FILE *out) {
    char line[READ_BUFFER_SIZE];
    FILE *fp;

    if (profile == NULL || out == NULL) {
        return 1;
    }

    fp = fopen(profile->path, "r");
    if (fp == NULL) {
        return 2;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        fputs(line, out);
    }

    fclose(fp);
    return 0;
}

int profile_validate_required(const firmware_profile_t *profile, char *missing, size_t missing_size) {
    int found[REQUIRED_COUNT] = {0, 0};
    char line[READ_BUFFER_SIZE];
    FILE *fp;
    size_t i;
    size_t used = 0;

    if (profile == NULL || missing == NULL || missing_size == 0U) {
        return 1;
    }

    missing[0] = '\0';
    fp = fopen(profile->path, "r");
    if (fp == NULL) {
        return 2;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        for (i = 0; i < REQUIRED_COUNT; ++i) {
            if (!found[i] && strstr(line, kRequiredTokens[i]) != NULL) {
                found[i] = 1;
            }
        }
    }

    fclose(fp);

    for (i = 0; i < REQUIRED_COUNT; ++i) {
        if (!found[i]) {
            const char *token = kRequiredTokens[i];
            size_t token_len = strlen(token);
            size_t extra = token_len + ((used > 0U) ? 2U : 0U);

            if (used + extra + 1U >= missing_size) {
                return 3;
            }

            if (used > 0U) {
                memcpy(missing + used, ", ", 2U);
                used += 2U;
            }

            memcpy(missing + used, token, token_len);
            used += token_len;
            missing[used] = '\0';
        }
    }

    return (used == 0U) ? 0 : 4;
}
