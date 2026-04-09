#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "miniaudio.h"
#define MP3_ID3_TAGS_IMPLEMENTATION
#include "mp3_id3_tags.h"
#include "simp_backend.h"

static ma_engine simp_gEngine;

struct simp_music {
    char *filename;
    ma_sound *sound;
    float length_in_second;
    bool has_tags;
    mp3_id3_tags tags;
}; 

bool
simp_init()
{
    ma_result result = ma_engine_init(NULL, &simp_gEngine);
    if (result != MA_SUCCESS) {
        return false;
    }
    return true;
}

void
simp_uninit()
{
    ma_engine_uninit(&simp_gEngine);
}

void
simp_music_destroy(Simp_Music music) 
{
    free(music->filename);
    ma_sound_uninit(music->sound);
    free(music->sound);
    free(music);
}

Simp_Music 
simp_music_new(char *filename) 
{
    Simp_Music music = malloc(sizeof(struct simp_music));
    music->filename = strdup(filename);
    music->sound = malloc(sizeof(ma_sound));
    ma_result result = ma_sound_init_from_file(&simp_gEngine, music->filename, 0, NULL, NULL, music->sound);
    if (result != MA_SUCCESS) {
        simp_music_destroy(music);
        return NULL;
    }
    result = ma_sound_get_length_in_seconds(music->sound, &music->length_in_second);
    if (result != MA_SUCCESS) {
        simp_music_destroy(music);
        return NULL;
    }
    music->has_tags = false;
    if (mp3_id3_has_tags(music->filename)) {
        music->has_tags = true;
        mp3_id3_read_tags(music->filename, &music->tags);
    }
    return music;
}

Simp_Result 
simp_music_play(Simp_Music music) 
{
    if (!music) return SIMP_INVALID_ARGS;
    if (ma_sound_start(music->sound) != MA_SUCCESS)
        return SIMP_ERROR;
    return SIMP_SUCCESS;
} 

Simp_Result
simp_music_pause(Simp_Music music)
{
    if (!music) return SIMP_INVALID_ARGS;
    if (ma_sound_stop(music->sound) != MA_SUCCESS)
        return SIMP_ERROR;
    return SIMP_SUCCESS;
}

Simp_Result
simp_music_get_length(Simp_Music music, float *out_second) 
{
    if (!music || !out_second) return SIMP_INVALID_ARGS;
    *out_second = music->length_in_second;
    return SIMP_SUCCESS;
} 

Simp_Result
simp_music_get_timestamp(Simp_Music music, float *out_second) 
{
    if (!music || !out_second) return SIMP_INVALID_ARGS;
    ma_result result = ma_sound_get_cursor_in_seconds(music->sound, out_second);
    if (result == MA_INVALID_ARGS) {
        return SIMP_INVALID_ARGS;
    }
    if (result != MA_SUCCESS) {
        return SIMP_ERROR;
    }
    return SIMP_SUCCESS;
}

Simp_Result
simp_music_set_timestamp(Simp_Music music, float second)
{
    if (!music) return SIMP_INVALID_ARGS;
    ma_result result = ma_sound_seek_to_second(music->sound, second);
    if (result == MA_INVALID_ARGS) {
        return SIMP_INVALID_ARGS;
    }
    if (result != MA_SUCCESS) {
        return SIMP_ERROR;
    }
    return SIMP_SUCCESS;
}

const char *
simp_music_title(Simp_Music music)
{
    if (!music->has_tags) return "Undefined";
    if (strlen(music->tags.title) == 0) return "Undefined";
    return music->tags.title;
}

const char *
simp_music_artist(Simp_Music music)
{
    if (!music->has_tags) return "Undefined";
    if (strlen(music->tags.artist) == 0) return "Undefined";
    return music->tags.artist;
}

const char *
simp_music_album(Simp_Music music)
{
    if (!music->has_tags) return "Undefined";
    if (strlen(music->tags.album) == 0) return "Undefined";
    return music->tags.album;
}

bool
simp_music_is_playing(Simp_Music music) 
{
    if (!music) return false;
    return ma_sound_is_playing(music->sound);
}
