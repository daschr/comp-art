#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    Vector2 pos;
    Vector2 dir;
    float rad;
    Color color;
} particle_t;


void update_particles(particle_t *particles, size_t num_particles, Vector2 *screendim);

int main(int ac, char *as[]) {
    if(ac!=3) {
        fprintf(stderr, "Usage: %s [size] [num particles]\n", as[0]);
        return EXIT_FAILURE;
    }

	int width, height;
	if(sscanf(as[1], "%dx%d", &width, &height)!=2){
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

    memset(particles, 0, sizeof(particle_t)*num_particles);
    srand((unsigned) time(NULL));

    Vector2 screendim= {width, height};
    InitWindow(width, height, "SPACE");
    HideCursor();

    SetTargetFPS(60);

    Color black= {.r=0, .b=0, .g=0};

    while(!WindowShouldClose()) {
        ClearBackground(black);
        screendim.x=GetScreenWidth();
        screendim.y=GetScreenHeight();

        update_particles(particles, num_particles, &screendim);
        BeginDrawing();
        for(size_t i=0; i<num_particles; ++i)
            DrawCircle(particles[i].pos.x, particles[i].pos.y, particles[i].rad, particles[i].color);
        //DrawFPS(10.0, 10.0);
		EndDrawing();
    }

    free(particles);
    CloseWindow();
    return EXIT_SUCCESS;
}

void update_particles(particle_t *particles, size_t num_particles, Vector2 *screendim) {
    for(size_t i=0; i<num_particles; ++i) {
        particle_t *p=particles+i;

        if(p->pos.x<=0.0 || p->pos.y<=0.0 || p->pos.x > screendim->x || p->pos.y > screendim->y) {
            p->dir.x=(GetRandomValue(0,1)?-1:1)* (0.05f + GetRandomValue(0,111)/31.0f);
            p->dir.y=(GetRandomValue(0,1)?-1:1)* (0.05f + GetRandomValue(0,111)/31.0f);
            p->pos.x=screendim->x/2.0+p->dir.x*(float) GetRandomValue(5,100);
            p->pos.y=screendim->y/2.0+p->dir.y*(float) GetRandomValue(5,100);
            p->rad=GetRandomValue(20,60)/20.0f;
            p->color=(Color) {
                .r=222.0f,
                .g=(float) GetRandomValue(127,222),
                .b=(float) GetRandomValue(67,167),
                .a=255.0f
            };
        } else {
            p->pos.x+=p->dir.x;
            p->pos.y+=p->dir.y;
        }
    }
}
