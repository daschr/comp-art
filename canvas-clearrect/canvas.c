#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define SPACE_BACKGROUND_COLOR 0xff000000

typedef struct {
    float pos[2];
    float dir[2];
    const char *color;
    uint8_t radius;
    uint8_t size;
} star_t;

typedef struct {
    double old_ts;
    int width;
    int height;
    int num_stars;

    char *canvas_id;
    star_t *stars;
} space_t;

inline void canvas_clearrect(const char *c_id, int width, int height);
inline void canvas_draw_filled_circle(const char *c_id, int32_t x, int32_t y, int32_t r, const char *color);

EMSCRIPTEN_KEEPALIVE space_t *initialize_2dspace(char *canvas_id, int width, int height, int num_stars) {
    srand(time(NULL));

	const char *color=strdup("white");
    space_t *s=malloc(sizeof(space_t));
    s->canvas_id=strdup(canvas_id);
    s->old_ts=0;
    s->width=width;
    s->height=height;
    s->num_stars=num_stars;
    s->stars=malloc(sizeof(star_t)*s->num_stars);

    float starting_time;
    for(int i=0; i<s->num_stars; ++i) {
        s->stars[i].dir[0]=(rand()&1?-1:1)*(0.1f+(((float) (rand()&4095))/4096.0f)); //px/ms
        s->stars[i].dir[1]=(rand()&1?-1:1)*(0.1f+(((float) (rand()&4095))/4096.0f)); //px/ms
        starting_time=(float) (rand()&2047);
        s->stars[i].pos[0]=(float) (width/2.f) + starting_time*s->stars[i].dir[0];
        s->stars[i].pos[1]=(float) (height/2.f) + starting_time*s->stars[i].dir[1];
        s->stars[i].color=color;
        s->stars[i].radius=(rand()&3)+1;
    }

    return s;
}

EMSCRIPTEN_KEEPALIVE void update_2dspace(space_t *s, double ts) {
    float starting_time;

    float delta_ts=!s->old_ts?1.0f:((float) (ts-s->old_ts)/4.f);
    s->old_ts=ts;

    canvas_clearrect(s->canvas_id, s->width, s->height);

    for(int i=0; i<s->num_stars; ++i) {
        canvas_draw_filled_circle(s->canvas_id, (int32_t) s->stars[i].pos[0], (int32_t) s->stars[i].pos[1], s->stars[i].radius, s->stars[i].color);
        s->stars[i].pos[0]+=delta_ts*s->stars[i].dir[0];
        s->stars[i].pos[1]+=delta_ts*s->stars[i].dir[1];


        if(s->stars[i].pos[0]>=(float)s->width | s->stars[i].pos[0]<0.f |
                s->stars[i].pos[1]>=(float)s->height|s->stars[i].pos[1]<0.f) {

            s->stars[i].dir[0]=(rand()&1?-1:1)*(0.1f+(((float) (rand()&4095))/4096.0f)); //px/ms
            s->stars[i].dir[1]=(rand()&1?-1:1)*(0.1f+(((float) (rand()&4095))/4096.0f)); //px/ms
            starting_time=(float) (rand()&2047);
            s->stars[i].pos[0]=(float) (s->width/2.f) + starting_time*s->stars[i].dir[0];
            s->stars[i].pos[1]=(float) (s->height/2.f) + starting_time*s->stars[i].dir[1];
            s->stars[i].radius=(rand()&3)+1;
//            s->stars[i].color=color;
        }
    }
}

inline void canvas_clearrect(const char *c_id, int width, int height) {
    EM_ASM({
        let ctx = document.getElementById(UTF8ToString($0)).getContext('2d');
        ctx.clearRect(0, 0, $0, $1);
    }, c_id, width, height);
}

inline void canvas_draw_filled_circle(const char *c_id, int32_t x, int32_t y, int32_t r, const char *color) {
    EM_ASM({
        let ctx = document.getElementById(UTF8ToString($0)).getContext('2d');
        ctx.beginPath();
        ctx.arc($1, $2, $3, 0, Math.PI*2, true);
        ctx.closePath();
        ctx.fillStyle = UTF8ToString($4);
        ctx.fill();
    }, c_id, x, y, r, color);
}
