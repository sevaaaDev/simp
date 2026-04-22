#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

#include "simp_backend.h"
#include "tui.h"

#define SIMP_VERSION "0.1.2b"

// these macros are from https://github.com/tsoding/nob.h, with some modification by me
#define DA_INIT_CAP 8
#define da_reserve(da, expected_capacity)                                              \
    do {                                                                                   \
        if ((expected_capacity) > (da)->capacity) {                                        \
            if ((da)->capacity == 0) {                                                     \
                (da)->capacity = DA_INIT_CAP;                                          \
            }                                                                              \
            while ((expected_capacity) > (da)->capacity) {                                 \
                (da)->capacity *= 2;                                                       \
            }                                                                              \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
        }                                                                                  \
    } while (0)

#define da_append(da, item)                \
    do {                                       \
        da_reserve((da), (da)->count + 1); \
        (da)->items[(da)->count++] = (item);   \
    } while (0)

struct Music_Entries {
    size_t capacity;
    size_t count;
    char **items;
};

bool read_dir_for_mp3(struct Music_Entries *entries, char *dirname);

int
main(int argc, char **argv)
{
    int ret = 0;
    if (argc == 1) {
        fprintf(stderr, "%s: need music filename as argument\n", argv[0]);
        fprintf(stderr, "USAGE:\n\t%s FILENAME\n\t%s OPTION\nOPTION:\n\t-v, --version\n", argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "--version") == 0 || 
        strcmp(argv[1], "-v") == 0) {
        printf("simp - simple music player v%s\n", SIMP_VERSION);
        return 0;
    }
    if (!simp_init()) {
        fprintf(stderr, "%s: error initialising backend\n", argv[0]);
        return 2;
    }
    // TODO: if argv[1] == directory, we read the dir (no recursion)
    // Simp_Music music = simp_music_new(argv[1]);
    // if (!music) {
    //     fprintf(stderr, "%s: error initialising music from file '%s'\n", argv[0], argv[1]);
    //     ret = 2;
    //     goto uninit_simp;
    // }

    struct Music_Entries entries = {};
    if (!read_dir_for_mp3(&entries, argv[1])) {
        fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
        return 1;
    }


    /////////////////
    // TUI SECTION //
    /////////////////


    // simp_music_destroy(music);
uninit_simp:
    simp_uninit();
    return ret;
    // TODO: investigate gdb cannot find bound after return
}

bool
read_dir_for_mp3(struct Music_Entries *entries, char *dirname)
{
    DIR *dir = opendir(dirname);
    if (!dir) return false;
    struct dirent *ent;
    int i = 0;
    errno = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (i > 1) {
            // TODO: store the filename if its mp3, also store the metadata
           // this mean we need to change the Simp_Music struct
            // {
            //     char *filename;
            //     struct {
                //     
            //     } metadata;
            //     ma_sound music_data;
            // };
            char *filename = strdup(ent->d_name);
            da_append(entries, filename);
        }
        i++;
        errno = 0;
    }
    closedir(dir);
    if (errno != 0) {
        return false;
    } 
    return true;
}
