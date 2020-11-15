#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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


void initialize_stars(RenderTexture2D *stars, size_t num_stars);
void initialize_particles(particle_t *particles, size_t num_particles, Vector2 *screendim);
void update_particles(particle_t *particles, size_t num_particles, Vector2 *screendim);

int main(int ac, char *as[]) {
    if(ac!=3) {
        fprintf(stderr, "Usage: %s [size] [num particles]\n", as[0]);
        return EXIT_FAILURE;
    }

    int width, height;
    if(sscanf(as[1], "%dx%d", &width, &height)!=2) {
        fprintf(stderr, "size %s is invalid!\n", as[1]);
        return EXIT_FAILURE;
    }

    size_t num_particles;
    if(sscanf(as[2], "%lu", &num_particles) != 1) {
        fprintf(stderr, "%s is not a positive number!\n", as[1]);
        return EXIT_FAILURE;
    }

    particle_t *particles=malloc(sizeof(particle_t)*num_particles);
    if(particles==NULL) {
        fprintf(stderr, "could not allocate enough space for %lu particles!\n", num_particles);
        return EXIT_FAILURE;
    }

	RenderTexture2D *stars=malloc(sizeof(RenderTexture2D)*NUM_STARS);
	if(stars==NULL){
			fprintf(stderr, "could not allocate space for textures!");
			free(particles);
			return EXIT_FAILURE;
	}
	
	memset(particles, 0, sizeof(particle_t)*num_particles);
    srand((unsigned) time(NULL));

    Vector2 screendim= {width, height};
    InitWindow(width, height, "SPACE");
    HideCursor();

    SetTargetFPS(60);

	initialize_stars(stars, NUM_STARS);
    initialize_particles(particles, num_particles, &screendim);
    
	while(!WindowShouldClose()) {
        ClearBackground(BLACK);
        screendim.x=GetScreenWidth();
        screendim.y=GetScreenHeight();

        update_particles(particles, num_particles, &screendim);

		BeginDrawing();
		for(size_t i=0;i<num_particles;++i)
			DrawTexture(stars[particles[i].star].texture, 
							particles[i].pos.x, particles[i].pos.y, particles[i].color);
		EndDrawing();
    }

	free(stars);
    free(particles);
    CloseWindow();
    return EXIT_SUCCESS;
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
