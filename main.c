#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "simp_backend.h"
#include "tui.h"

#define SIMP_VERSION "0.1.2b"

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

    DIR *dir = opendir(argv[1]);
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
            printf("%s\n", ent->d_name);
        }
        i++;
        errno = 0;
    }
    if (errno != 0) {
        fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
        goto uninit_simp;
    } 

    /////////////////
    // TUI SECTION //
    /////////////////


    // simp_music_destroy(music);
uninit_simp:
    closedir(dir);
    simp_uninit();
    return ret;
}
