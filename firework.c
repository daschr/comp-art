#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

typedef struct {
    Vector2 pos;
    Vector2 dir;
    float rad;
	int8_t opstep;
	Color color;
} particle_t;

size_t update_particles(particle_t *particles, size_t num_particles, int8_t no_new, Vector2 pos, float sin_x, float cos_x);

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
    InitWindow(width, height, "FIREWORK");
    HideCursor();

    SetTargetFPS(60);

    Color black= {.r=0, .b=0, .g=0};

	Vector2 rocket_pos={.x=screendim.x/2.0, .y=screendim.y};
	float dir=PI/2.0;
	float sin_d=0.0f, cos_d=0.0f;
	int exploded=0;
	float rocket_height=0.0f;
	Vector2 speed={0.0f, 2.0f};

	while(!WindowShouldClose()) {
        ClearBackground(black);
        screendim.x=GetScreenWidth();
        screendim.y=GetScreenHeight();

        if(rocket_height==0.0f || !update_particles(particles, num_particles, exploded, rocket_pos, sin_d, cos_d)){
				rocket_pos=(Vector2) {.x=GetRandomValue(0,screendim.x), .y=screendim.y+1};
				dir=GetRandomValue(155, 205)/180.0*PI;
				sin_d=sin(dir);
				cos_d=cos(dir);
				exploded=0;
				rocket_height=(float) (screendim.y/10+GetRandomValue(0,2*screendim.y/3)); 
				update_particles(particles, num_particles, exploded, rocket_pos, sin(dir), cos(dir));
		}
		
		rocket_pos.x+=cos_d*speed.x - sin_d*speed.y;
		rocket_pos.y+=sin_d*speed.x + cos_d*speed.y;
		if(rocket_pos.y<=rocket_height)
				exploded=1;

        BeginDrawing();
        for(size_t i=0; i<num_particles; ++i)
           	DrawCircle(particles[i].pos.x, particles[i].pos.y, particles[i].rad, particles[i].color);
        DrawFPS(10.0, 10.0);
		EndDrawing();
    }

    free(particles);
    CloseWindow();
    return EXIT_SUCCESS;
}

size_t update_particles(particle_t *particles, size_t num_particles, int8_t no_new, Vector2 pos, float sin_x, float cos_x) {
	size_t n_updated=0;
	for(size_t i=0; i<num_particles; ++i) {
        particle_t *p=particles+i;

        if(p->color.a<=p->opstep) {
            if(no_new)
					continue;
			float x=(GetRandomValue(0,1)?-1:1)* (0.05f + GetRandomValue(0,10)/31.0f);
            float y=(0.05f + GetRandomValue(0,111)/31.0f);
            p->dir.x=cos_x*x-sin_x*y;
			p->dir.y=sin_x*x+cos_x*y;
			p->pos.x=pos.x;
            p->pos.y=pos.y;
            p->rad=GetRandomValue(1,2)/2.0f;
			p->opstep=(int8_t) ((x<0?-x:x)/(0.05f+10.0/31.0f)*10.0f);
            p->color=(Color) {
                .r=222.0f,
                .g=(float) GetRandomValue(127,222),
                .b=(float) GetRandomValue(67,167),
                .a=255
            };
        } else {
            p->pos.x-=p->dir.x;
            p->pos.y-=p->dir.y;
			p->color.a-=p->opstep;
        	++n_updated;
		}
    }
	return n_updated;
}
