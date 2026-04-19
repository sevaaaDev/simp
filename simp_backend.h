// ************************************************************
// * Example                                                  *
// ************************************************************
// int
// main()
// {
//     simp_init();
//     struct music *music = simp_new_music("girl-scout.mp3");
//     simp_play_music(music);
//     while (simp_is_playing(music)) {
//         float sec;
//         simp_get_current_timestamp(music, &sec);
//         printf("%f\n", sec);
//         sleep(1);
//     }
//     simp_stop_music(music);
//     simp_destroy_music(music);
//     simp_uninit();
// }

#ifndef SIMP_BACKEND_H
#define SIMP_BACKEND_H

#include "miniaudio.h"

// play file
// pause & play
// seek to specific timestamp
// get the current timestamp

/*  simp FILENAME
 *  :stop
 *  :start
 *  :seek 5 3
 *  :bar
 * */

// BUG: i think this isnt how opaque works
typedef struct simp_music * Simp_Music;

typedef enum {
    SIMP_SUCCESS,
    SIMP_ERROR,
    SIMP_INVALID_ARGS,
} Simp_Result;

bool             simp_init();
void             simp_uninit();
Simp_Music       simp_music_new(char *name);
void             simp_music_destroy(Simp_Music music);
const char *     simp_music_title(Simp_Music music);
const char *     simp_music_artist(Simp_Music music);
const char *     simp_music_album(Simp_Music music);
bool             simp_music_is_playing(Simp_Music music);
Simp_Result      simp_music_play(Simp_Music music);
Simp_Result      simp_music_pause(Simp_Music music);
Simp_Result      simp_music_get_length(Simp_Music music, float *out_second);
Simp_Result      simp_music_get_timestamp(Simp_Music music, float *out_second);
Simp_Result      simp_music_set_timestamp(Simp_Music music, float second);

#endif
