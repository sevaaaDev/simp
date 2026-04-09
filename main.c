#include <stdio.h>
#include "simp_backend.h"

int
main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "%s: need music filename as argument\n", argv[0]);
        fprintf(stderr, "USAGE:\n\t%s FILENAME\n", argv[0]);
        return 1;
    }
    if (!simp_init()) {
        fprintf(stderr, "%s: error initialising backend\n", argv[0]);
        return 1;
    }
    Simp_Music music = simp_music_new(argv[1]);
    if (!music) {
        fprintf(stderr, "%s: error initialising music from file '%s'\n", argv[0], argv[1]);
        return 1;
    }
    const char *title, *artist, *album;
    title = simp_music_title(music);
    artist = simp_music_artist(music);
    album = simp_music_album(music);
    printf("Playing %s - %s - %s\n", artist, album, title);
    float len;
    simp_music_get_length(music, &len);
    printf("music length: %f\n", len);

    simp_music_destroy(music);
    simp_uninit();
}
