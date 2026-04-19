#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <math.h> // for fmod
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
    Simp_Music music = simp_music_new(argv[1]);
    if (!music) {
        fprintf(stderr, "%s: error initialising music from file '%s'\n", argv[0], argv[1]);
        ret = 2;
        goto uninit_simp;
    }
    setlocale(LC_ALL, "");
    /////////////////
    // TUI SECTION //
    /////////////////

    endwin();

    simp_music_destroy(music);
uninit_simp:
    simp_uninit();
    return ret;
}
