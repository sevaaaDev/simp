#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <math.h> // for fmod
#include "simp_backend.h"

#define SIMP_VERSION "0.1.2b"

void render_text(int y, int x, int height, int width, const char *text, int text_len);

// TODO: is it the time to make progress_bar struct?
void
render_progress_bar(const char *artist,
                    const char *album,
                    const char *title,
                    float total_len, float timestamp, const char *state)
{
    int y = 0, x = 0, width = COLS;

    int total_minute = total_len / 60.f;
    int total_second = fmod(total_len, 60.f);
    int current_minute = timestamp / 60.f;
    int current_second = fmod(timestamp, 60.f);
    int n = snprintf(NULL, 0, "%02d:%02d / %02d:%02d", 
                    current_minute, 
                    current_second, 
                    total_minute, 
                    total_second);
    char buf[n+1] = {};
    snprintf(buf, n+1, "%02d:%02d / %02d:%02d", 
                    current_minute, 
                    current_second, 
                    total_minute, 
                    total_second);
    int x_start_timestamp = width - n;
    mvprintw(y, x_start_timestamp, "%s", buf);

    // NOTE: this is refactor candidate
    n = snprintf(NULL, 0, "%s: %s - %s - %s", state, artist, album, title);
    char music_info[n+1] = {};
    snprintf(music_info, n+1, "%s: %s - %s - %s", state, artist, album, title);
    render_text(y, x, 1, x_start_timestamp-1, music_info, n);

    int progress_char_len = timestamp / total_len * width;
    attron(A_REVERSE);
    mvprintw(y+1, x, "%*s", progress_char_len, " ");
    attroff(A_REVERSE);
}

void
render_text(int y, int x, int height, int width, const char *text, int text_len)
{
    // placeholder_text = "The Saturn V rocket was a powerful launch vehicle developed by NASA to carry astronauts to the Moon during Apollo missions";
    int x_offset = 0;
    int y_offset = 0;
    for (int i = 0; i < text_len; i++) {
        mvaddch(y+y_offset, x+x_offset, text[i]);
        x_offset++;
        if (x_offset >= width) {
            x_offset = 0;
            y_offset++;
            if (y_offset >= height) {
                mvaddstr(y+height-1, x+width-1, "…");
                break;
            }
        }
    }
}


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
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    timeout(100);

    int tick = 0;
    bool should_render = true;
    while (true) {
        // TODO: this loop is looping per 100ms, is that okay?
        int c = getch();
        if (c == 'q') {
            break;
        }
        switch (c) {
            // TODO: whats the best way to handle keybind? bcs we must print it as well on help box
        case 'p':
            // TODO: make simp_toggle_music() in backend
            if (simp_music_is_playing(music))
                simp_music_pause(music);
            else
                simp_music_play(music);
            break;
        }

        // BUG: the state rendered will stay PLAYING: after music end
        if (simp_music_is_playing(music)) tick++;
        if (c > -1) should_render = true;
        if (tick >= 10) {
            tick = 0;
            should_render = true;
        }
        if (should_render) {
            should_render = false;

            clear();

            float timestamp, total_len;
            simp_music_get_length(music, &total_len);
            simp_music_get_timestamp(music, &timestamp);
            render_progress_bar(simp_music_artist(music),
                                simp_music_album(music), 
                                simp_music_title(music),
                                total_len, timestamp,
                                simp_music_is_playing(music) ? "Playing" : "Paused");
            #define string(str) str, sizeof(str) - 1
            render_text(2, 0, 1, COLS, string("Pause/Play: p | Quit: q"));
            // TODO: make music list functional
            // render_text(2, 0, 1, COLS, "Men I Trust - Days Go By", 24);
            // render_text(3, 0, 1, COLS, "Men I Trust - Oncle Jazz", 24);
            // render_text(4, 0, 1, COLS, "Girl Scout - Honey", 18);
        }
    }

    endwin();

    simp_music_destroy(music);
uninit_simp:
    simp_uninit();
    return ret;
}
