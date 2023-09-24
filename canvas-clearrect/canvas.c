#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define SPACE_BACKGROUND_COLOR 0xff000000

typedef struct {
    double pos[2];
    double dir[2];
    const char *color;
    uint8_t radius;
    uint8_t size;
} star_t;

typedef struct {
    double old_ts;
    int num_stars;

    char *canvas_id;
    star_t *stars;
} space_t;

inline void canvas_clearrect(const char *c_id, int width, int height);
inline void canvas_draw_filled_circle(const char *c_id, int32_t x, int32_t y, int32_t r, const char *color);

EMSCRIPTEN_KEEPALIVE space_t *initialize_2dspace(char *canvas_id, int num_stars) {
    srand(time(NULL));

    const char *color=strdup("white");
    space_t *s=malloc(sizeof(space_t));
    s->canvas_id=strdup(canvas_id);
    s->old_ts=0;
    s->num_stars=num_stars;
    s->stars=malloc(sizeof(star_t)*s->num_stars);

    for(int i=0; i<s->num_stars; ++i) {
        s->stars[i].dir[0]=(rand()&1?-1.:1.)*(0.00001+(((double) (rand()&8191))/8192.0)/2000.0);
        s->stars[i].dir[1]=(rand()&1?-1.:1.)*(0.00001+(((double) (rand()&8191))/8192.0)/2000.0);
        const double starting_time=(double) (rand()&((1<<8)-1));
        s->stars[i].pos[0]=starting_time*s->stars[i].dir[0];
        s->stars[i].pos[1]=starting_time*s->stars[i].dir[1];
        s->stars[i].color=color;
        s->stars[i].radius=(rand()&3)+1;
    }

    return s;
}

EMSCRIPTEN_KEEPALIVE void update_2dspace(space_t *s, double ts) {
    double delta_ts=!s->old_ts?1.0:(ts-s->old_ts);
    s->old_ts=ts;

    const int32_t height=EM_ASM_INT({return document.getElementById(UTF8ToString($0)).height}, s->canvas_id),
                  width=EM_ASM_INT({return document.getElementById(UTF8ToString($0)).width}, s->canvas_id);
    const int32_t height_h=height>>1, width_h=width>>1;

    canvas_clearrect(s->canvas_id, width, height);

    for(int i=0; i<s->num_stars; ++i) {
        canvas_draw_filled_circle(s->canvas_id, (int32_t) (width_h+s->stars[i].pos[0]*width_h), (int32_t) (height_h+s->stars[i].pos[1]*height_h), s->stars[i].radius, s->stars[i].color);
        s->stars[i].pos[0]+=delta_ts*s->stars[i].dir[0];
        s->stars[i].pos[1]+=delta_ts*s->stars[i].dir[1];

        if((s->stars[i].pos[0]>1.0)|(s->stars[i].pos[0]<-1.0)|
                (s->stars[i].pos[1]>1.0)|(s->stars[i].pos[1]<-1.0)) {

            s->stars[i].dir[0]=(rand()&1?-1.:1.)*(0.00001+(((double) (rand()&8191))/8192.0)/2000.0);
            s->stars[i].dir[1]=(rand()&1?-1.:1.)*(0.00001+(((double) (rand()&8191))/8192.0)/2000.0);
            const double starting_time=(double) (rand()&((1<<8)-1));
            s->stars[i].pos[0]=starting_time*s->stars[i].dir[0];
            s->stars[i].pos[1]=starting_time*s->stars[i].dir[1];
            s->stars[i].radius=(rand()&3)+1;
        }
    }
}

inline void canvas_clearrect(const char *c_id, int width, int height) {
    EM_ASM({
        let ctx = document.getElementById(UTF8ToString($0)).getContext('2d');
        ctx.clearRect(0, 0, $1, $2);
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
