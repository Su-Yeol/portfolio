#define _POSIX_C_SOURCE 200809L

#include "profile_catalog.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define EXIT_OK 0
#define EXIT_FAIL 1
#define EXIT_USAGE 2
#define MISSING_BUFFER_SIZE 128

static void print_usage(const char *program) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s list\n", program);
    fprintf(stderr, "  %s show <profile-name>\n", program);
    fprintf(stderr, "  %s validate\n", program);
}

static int switch_to_executable_dir(void) {
    char exe_path[PATH_MAX];
    ssize_t len;
    char *last_sep;

    len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len <= 0) {
        return 1;
    }

    exe_path[len] = '\0';
    last_sep = strrchr(exe_path, '/');
    if (last_sep == NULL) {
        return 2;
    }

    *last_sep = '\0';
    if (chdir(exe_path) != 0) {
        return 3;
    }

    return 0;
}

static int cmd_list(void) {
    size_t i;

    for (i = 0; i < profile_count(); ++i) {
        const firmware_profile_t *profile = profile_at(i);
        if (profile != NULL) {
            printf("[%s] %s\n", profile->category, profile->name);
        }
    }

    return EXIT_OK;
}

static int cmd_show(const char *name) {
    const firmware_profile_t *profile = profile_find(name);
    if (profile == NULL) {
        fprintf(stderr, "Profile not found: %s\n", name);
        return EXIT_FAIL;
    }

    if (profile_show(profile, stdout) != 0) {
        fprintf(stderr, "Unable to read profile source: %s\n", profile->path);
        return EXIT_FAIL;
    }

    return EXIT_OK;
}

static int cmd_validate(void) {
    size_t i;
    int failed = 0;

    for (i = 0; i < profile_count(); ++i) {
        char missing[MISSING_BUFFER_SIZE];
        const firmware_profile_t *profile = profile_at(i);
        int rc;

        if (profile == NULL) {
            continue;
        }

        rc = profile_validate_required(profile, missing, sizeof(missing));
        if (rc == 0) {
            printf("PASS [%s] %s\n", profile->category, profile->name);
            continue;
        }

        failed = 1;
        if (rc == 4) {
            printf("FAIL [%s] %s: missing %s\n", profile->category, profile->name, missing);
        } else {
            printf("FAIL [%s] %s: unable to validate\n", profile->category, profile->name);
        }
    }

    return failed ? EXIT_FAIL : EXIT_OK;
}

int main(int argc, char **argv) {
    if (switch_to_executable_dir() != 0) {
        fprintf(stderr, "Unable to initialize execution directory\n");
        return EXIT_FAIL;
    }

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_USAGE;
    }

    if (strcmp(argv[1], "list") == 0) {
        return cmd_list();
    }

    if (strcmp(argv[1], "show") == 0) {
        if (argc < 3) {
            print_usage(argv[0]);
            return EXIT_USAGE;
        }
        return cmd_show(argv[2]);
    }

    if (strcmp(argv[1], "validate") == 0) {
        return cmd_validate();
    }

    print_usage(argv[0]);
    return EXIT_USAGE;
}
