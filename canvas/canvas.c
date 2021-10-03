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
    uint32_t color;
    uint8_t radius;
    uint8_t size;
} star_t;

typedef struct {
    int width;
    int height;
    int num_stars;
    size_t imgbuf_s;

    char *canvas_id;
    star_t *stars;
    uint32_t *imgbuf;

} space_t;

void save_canvas(const char *c_id);
void restore_canvas(const char *c_id);
void update_canvas(const char *c_id, const uint32_t *frame, const int width, const int height);
void update_2dspace_animation(void *s_r);

inline void fill_imgbuf(uint32_t *imgbuf, size_t imgbuf_size, uint32_t color) {
    for(size_t i=0; i<imgbuf_size; ++i)
        imgbuf[i]=color;
}

void draw_filled_circle(uint32_t *imgbuf, int32_t width, int32_t height,
                        int32_t x, int32_t y, int32_t r, uint32_t color) {
    int32_t p_x, p_y;
    const uint64_t r_2=r*r;
    for(uint32_t x_i=0; x_i<=r; ++x_i) {
        for(uint32_t y_i=0; y_i<=r; ++y_i) {
            if((uint64_t) (x_i*x_i+y_i*y_i)<=r_2) {
                p_x=x-x_i;
                p_y=y-y_i;
                if(p_y>=0&p_y<height&p_x>=0&p_x<width)
                    imgbuf[width*p_y+p_x]=color;
                p_x=x+x_i;
                if(p_y>=0&p_y<height&p_x>=0&p_x<width)
                    imgbuf[width*p_y+p_x]=color;
                p_y=y+y_i;
                if(p_y>=0&p_y<height&p_x>=0&p_x<width)
                    imgbuf[width*p_y+p_x]=color;
                p_x=x-x_i;
                if(p_y>=0&p_y<height&p_x>=0&p_x<width)
                    imgbuf[width*p_y+p_x]=color;
            }
        }
    }
}

EMSCRIPTEN_KEEPALIVE void animate_2dspace(char *canvas_id, int width, int height, int num_stars) {
    srand(time(NULL));

    space_t *s=malloc(sizeof(space_t));
    s->canvas_id=strdup(canvas_id);
    s->width=width;
    s->height=height;
    s->imgbuf_s=width*height;
    s->num_stars=num_stars;
    s->imgbuf=malloc(sizeof(uint32_t)*s->imgbuf_s);
    s->stars=malloc(sizeof(star_t)*s->num_stars);
    for(int i=0; i<s->num_stars; ++i) {
        s->stars[i].dir[0]=(0.1f+(((float) (rand()&255))/128.0f));
        s->stars[i].dir[1]=(0.1f+(((float) (rand()&255))/128.0f));
        s->stars[i].dir[0]=rand()&1?-s->stars[i].dir[0]:s->stars[i].dir[0];
        s->stars[i].dir[1]=rand()&1?-s->stars[i].dir[1]:s->stars[i].dir[1];
        s->stars[i].pos[0]=(float) (width/2.f) + (float) (rand()&127)*s->stars[i].dir[0];
        s->stars[i].pos[1]=(float) (height/2.f) + (float) (rand()&127)*s->stars[i].dir[1];
        s->stars[i].color=0xffffffff;
        s->stars[i].radius=(rand()&3)+1;
    }
	
	emscripten_set_main_loop_arg(update_2dspace_animation, (void *) s, 0, 0);
}

void update_2dspace_animation(void *s_r) {
    space_t *s=(space_t *) s_r;
    float starting_time;
    
	fill_imgbuf(s->imgbuf, s->imgbuf_s, SPACE_BACKGROUND_COLOR);

	for(int i=0; i<s->num_stars; ++i) {
		draw_filled_circle(s->imgbuf, s->width, s->height, (int32_t) s->stars[i].pos[0], (int32_t) s->stars[i].pos[1], s->stars[i].radius, s->stars[i].color);
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
            s->stars[i].color=0xffffffff;
            s->stars[i].radius=(rand()&3)+1;
        }
    }

    update_canvas(s->canvas_id, s->imgbuf, s->width, s->height);
}

void save_canvas(const char *c_id){
    EM_ASM({
        let context = document.getElementById(UTF8ToString($0)).getContext('2d');
        context.save();
    }, c_id);
}

void restore_canvas(const char *c_id){
    EM_ASM({
        let context = document.getElementById(UTF8ToString($0)).getContext('2d');
        context.restore();
    }, c_id);
}

void update_canvas(const char *c_id, const uint32_t *frame, int width, int height) {
    EM_ASM({
        let data = Module.HEAPU8.slice($1, $1 + $2 * $3 * 4);
        let context = document.getElementById(UTF8ToString($0)).getContext('2d');
        let imageData = context.getImageData(0, 0, $2, $3);
        imageData.data.set(data);
        context.putImageData(imageData, 0, 0);
    }, c_id, frame, width, height);
}
