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
    uint8_t radius;
    uint8_t size;
    char *color;
} star_t;

typedef struct {
    int width;
    int height;
    int num_stars;

    char *canvas_id;
    star_t *stars;
} space_t;

void save_canvas(const char *c_id);
void restore_canvas(const char *c_id);
void update_2dspace_animation(void *s_r);

inline void canvas_draw_filled_circle(const char *c_id, int32_t x, int32_t y, int32_t r, const char *color);
inline void canvas_clearrect(const char *c_id, int width, int height);

EMSCRIPTEN_KEEPALIVE void animate_2dspace(char *canvas_id, int width, int height, int num_stars) {
    srand(time(NULL));

    char *color=strdup("white");
    space_t *s=malloc(sizeof(space_t));
    s->canvas_id=strdup(canvas_id);
    s->width=width;
    s->height=height;
    s->num_stars=num_stars;
    s->stars=malloc(sizeof(star_t)*s->num_stars);

    for(int i=0; i<s->num_stars; ++i) {
        s->stars[i].dir[0]=(0.1f+(((float) (rand()&255))/128.0f));
        s->stars[i].dir[1]=(0.1f+(((float) (rand()&255))/128.0f));
        s->stars[i].dir[0]=rand()&1?-s->stars[i].dir[0]:s->stars[i].dir[0];
        s->stars[i].dir[1]=rand()&1?-s->stars[i].dir[1]:s->stars[i].dir[1];
        s->stars[i].pos[0]=(float) (width/2.f) + (float) (rand()&127)*s->stars[i].dir[0];
        s->stars[i].pos[1]=(float) (height/2.f) + (float) (rand()&127)*s->stars[i].dir[1];
        s->stars[i].color=color;
        s->stars[i].radius=(rand()&3)+1;
    }

    emscripten_set_main_loop_arg(update_2dspace_animation, (void *) s, 0, 0);
}

void update_2dspace_animation(void *s_r) {
    space_t *s=(space_t *) s_r;
    float starting_time;

    canvas_clearrect(s->canvas_id, (int32_t) s->width, (int32_t) s->height);

    for(int i=0; i<s->num_stars; ++i) {
        canvas_draw_filled_circle(s->canvas_id, (int32_t) s->stars[i].pos[0], (int32_t) s->stars[i].pos[1], s->stars[i].radius, s->stars[i].color);
        s->stars[i].pos[0]+=s->stars[i].dir[0];
        s->stars[i].pos[1]+=s->stars[i].dir[1];

        if(s->stars[i].pos[0]>=(float)s->width | s->stars[i].pos[0]<0.f |
                s->stars[i].pos[1]>=(float)s->height|s->stars[i].pos[1]<0.f) {

            s->stars[i].dir[0]=(0.1f+(((float) (rand()&255))/128.0f));
            s->stars[i].dir[1]=(0.1f+(((float) (rand()&255))/128.0f));
            s->stars[i].dir[0]=rand()&1?-s->stars[i].dir[0]:s->stars[i].dir[0];
            s->stars[i].dir[1]=rand()&1?-s->stars[i].dir[1]:s->stars[i].dir[1];
            starting_time=(float) (rand()&127);
            s->stars[i].pos[0]=(float) (s->width/2.f) + starting_time*s->stars[i].dir[0];
            s->stars[i].pos[1]=(float) (s->height/2.f) + starting_time*s->stars[i].dir[1];
            s->stars[i].radius=(rand()&3)+1;
        }
    }
}

void save_canvas(const char *c_id) {
    EM_ASM({
        let context = document.getElementById(UTF8ToString($0)).getContext('2d');
        context.save();
    }, c_id);
}

void restore_canvas(const char *c_id) {
    EM_ASM({
        let context = document.getElementById(UTF8ToString($0)).getContext('2d');
        context.restore();
    }, c_id);
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
