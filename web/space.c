#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <emscripten/emscripten.h>

#define NUM_PARTICLES 200

#ifndef NUM_STARS
#define NUM_STARS 10
#endif

#ifndef BIGGEST_STAR_RAD
#define BIGGEST_STAR_RAD 3
#endif

typedef struct {
    Vector2 pos;
    Vector2 dir;
    int star;
    Color color;
} particle_t;

size_t num_particles;
Vector2 screendim;
particle_t *particles;
RenderTexture2D *stars;

void update_frame(void);
void initialize_stars(RenderTexture2D *stars, size_t num_stars);
void initialize_particles(particle_t *particles, size_t num_particles, Vector2 *screendim);
void update_particles(particle_t *particles, size_t num_particles, Vector2 *screendim);

int main(int ac, char *as[]) {
	num_particles=NUM_PARTICLES;

    particles=malloc(sizeof(particle_t)*num_particles);
    if(particles==NULL) {
        fprintf(stderr, "could not allocate enough space for %lu particles!\n", num_particles);
        return EXIT_FAILURE;
    }

	stars=malloc(sizeof(RenderTexture2D)*NUM_STARS);
	if(stars==NULL){
			fprintf(stderr, "could not allocate space for textures!");
			free(particles);
			return EXIT_FAILURE;
	}

	memset(particles, 0, sizeof(particle_t)*num_particles);
    srand((unsigned) time(NULL));

    //screendim= (Vector2) {GetScreenWidth(), GetScreenHeight()};
    screendim= (Vector2) {1920.0f, 1080.0f};
	
	InitWindow(screendim.x, screendim.y, "SPACE");
    HideCursor();

	initialize_stars(stars, NUM_STARS);
    initialize_particles(particles, num_particles, &screendim);

	emscripten_set_main_loop(update_frame, 0, 1);	

	free(stars);
    free(particles);
    CloseWindow();
    return EXIT_SUCCESS;
}

void update_frame(void){
		ClearBackground(BLACK);
        //screendim.x=GetScreenWidth();
        //screendim.y=GetScreenHeight();

        update_particles(particles, num_particles, &screendim);

		BeginDrawing();
		for(size_t i=0;i<num_particles;++i)
			DrawTexture(stars[particles[i].star].texture, 
							particles[i].pos.x, particles[i].pos.y, particles[i].color);
		EndDrawing();
}

void initialize_stars(RenderTexture2D *stars, size_t num_stars){
	float rad=(float) BIGGEST_STAR_RAD;
	for(size_t i=0;i<num_stars;++i){
		stars[i]=LoadRenderTexture(BIGGEST_STAR_RAD*2+1, BIGGEST_STAR_RAD*2+1);
		BeginTextureMode(stars[i]);
		DrawCircle(BIGGEST_STAR_RAD+1, BIGGEST_STAR_RAD+1, rad, (Color) { 200.0f, 200.0f, 200.0f, 255.0f });
		EndTextureMode();
		rad-=(float) (BIGGEST_STAR_RAD-1)/ (float) num_stars;
	}
}

void initialize_particles(particle_t *particles, size_t num_particles, Vector2 *screendim) {
    particle_t *p;
    for(size_t i=0; i<num_particles; ++i) {
        p=particles+i;

        p->dir.x=(GetRandomValue(0,1)?-1:1)* (0.05f + GetRandomValue(0,111)/31.0f);
        p->dir.y=(GetRandomValue(0,1)?-1:1)* (0.05f + GetRandomValue(0,111)/31.0f);
        p->pos.x=screendim->x/2.0+p->dir.x*(float) GetRandomValue(-5,5);
        p->pos.y=screendim->y/2.0+p->dir.y*(float) GetRandomValue(-5,5);
        p->star=GetRandomValue(0,NUM_STARS);
        float c=(float)GetRandomValue(200,255);
        p->color=(Color) {
            .r=c, .g=c, .b=c, .a=255.0f
        };
    }
}

void update_particles(particle_t *particles, size_t num_particles, Vector2 *screendim) {
    particle_t *p;
    for(size_t i=0; i<num_particles; ++i) {
        p=particles+i;
        if(	p->pos.x < 0 || p->pos.y < 0
                || p->pos.x > screendim->x || p->pos.y > screendim->y) {

            p->pos.x=screendim->x/2.0+p->dir.x*2.0;
            p->pos.y=screendim->y/2.0+p->dir.y*2.0;
        }

        p->pos.x+=p->dir.x;
        p->pos.y+=p->dir.y;
    }
}
