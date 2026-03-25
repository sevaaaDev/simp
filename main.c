#include "miniaudio.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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


static ma_engine gEngine;

struct music {
    char *name;
    ma_sound *sound;
    float length_in_second;
};

typedef enum {
    SIMP_success,
    SIMP_error,
    SIMP_invalid_args,
} simp_result;

bool             simp_init();
void             simp_uninit();
void             simp_destroy_music(struct music *music);
struct music *   simp_new_music(char *name);
void             simp_play_music(struct music *music);
void             simp_stop_music(struct music *music);
simp_result      simp_get_current_timestamp(struct music *music, float *cursor);
simp_result      simp_seek_to_second(struct music *music, unsigned int second);
bool             simp_is_playing(struct music *music);

bool
simp_init()
{
    ma_result result = ma_engine_init(NULL, &gEngine);
    if (result != MA_SUCCESS) {
        return false;
    }
    return true;
}

void
simp_uninit()
{
    ma_engine_uninit(&gEngine);
}

void
simp_destroy_music(struct music *music) 
{
    free(music->name);
    ma_sound_uninit(music->sound);
    free(music->sound);
    free(music);
}

struct music *
simp_new_music(char *name) 
{
    struct music *music = malloc(sizeof(struct music));
    music->name = strdup(name);
    music->sound = malloc(sizeof(ma_sound));
    ma_result result = ma_sound_init_from_file(&gEngine, music->name, 0, NULL, NULL, music->sound);
    if (result != MA_SUCCESS) {
        simp_destroy_music(music);
        return NULL;
    }
    result = ma_sound_get_length_in_seconds(music->sound, &music->length_in_second);
    if (result != MA_SUCCESS) {
        simp_destroy_music(music);
        return NULL;
    }
    return music;
}

void 
simp_play_music(struct music *music) 
{
    ma_sound_start(music->sound);
} 

void
simp_stop_music(struct music *music)
{
    ma_sound_stop(music->sound);
}

simp_result
simp_get_current_timestamp(struct music *music, float *cursor) 
{
    if (!music || !cursor) return SIMP_invalid_args;
    ma_result result = ma_sound_get_cursor_in_seconds(music->sound, cursor);
    if (result == MA_INVALID_ARGS) {
        return SIMP_invalid_args;
    }
    if (result != MA_SUCCESS) {
        return SIMP_error;
    }
    return SIMP_success;
}

simp_result
simp_seek_to_second(struct music *music, unsigned int second)
{
    if (!music) return SIMP_invalid_args;
    ma_result result = ma_sound_seek_to_second(music->sound, second);
    if (result == MA_INVALID_ARGS) {
        return SIMP_invalid_args;
    }
    if (result != MA_SUCCESS) {
        return SIMP_error;
    }
    return SIMP_success;
}

bool
simp_is_playing(struct music *music) 
{
    return ma_sound_is_playing(music->sound);
}

int
main()
{
    simp_init();
    struct music *music = simp_new_music("girl-scout.mp3");
    simp_play_music(music);
    while (simp_is_playing(music)) {
        float sec;
        simp_get_current_timestamp(music, &sec);
        printf("%f\n", sec);
        sleep(1);
    }
    simp_stop_music(music);
    simp_destroy_music(music);
    simp_uninit();
}
