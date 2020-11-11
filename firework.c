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

typedef struct {
	Vector2 pos;
	Vector2 dir;
	particle_t *particles;
	size_t num_particles;
	float sin_d, cos_d;
	bool exploded;
	float height;
} rocket_t;

void update_rockets(rocket_t *rockets, size_t num_rockets, Vector2 screendim);
size_t update_particles(particle_t *particles, size_t num_particles, int8_t no_new, Vector2 pos, float sin_x, float cos_x);
void free_rockets(rocket_t *rockets, size_t num_rockets);
rocket_t *initialize_rockets(size_t num_rockets, size_t num_particles);

int main(int ac, char *as[]) {
    if(ac!=4) {
        fprintf(stderr, "Usage: %s [size] [num rockets] [num particles]\n", as[0]);
        return EXIT_FAILURE;
    }

	int width, height;
	if(sscanf(as[1], "%dx%d", &width, &height)!=2){
			fprintf(stderr, "size %s is invalid!\n", as[1]);
			return EXIT_FAILURE;
	}

    size_t num_rockets;
    if(sscanf(as[2], "%lu", &num_rockets) != 1) {
        fprintf(stderr, "%s is not a positive number!\n", as[1]);
        return EXIT_FAILURE;
    }

    size_t num_particles;
    if(sscanf(as[3], "%lu", &num_particles) != 1) {
        fprintf(stderr, "%s is not a positive number!\n", as[1]);
        return EXIT_FAILURE;
    }

	rocket_t *rockets;
	if((rockets=initialize_rockets(num_rockets, num_particles))==NULL)
			return EXIT_FAILURE;
    
	srand((unsigned) time(NULL));

    Vector2 screendim= {width, height};
    InitWindow(width, height, "FIREWORK");
    HideCursor();

    SetTargetFPS(60);

    Color black= {.r=0, .b=0, .g=0};

	while(!WindowShouldClose()) {
        ClearBackground(black);
        screendim.x=GetScreenWidth();
        screendim.y=GetScreenHeight();

		update_rockets(rockets, num_rockets, screendim);
        BeginDrawing();
		for(size_t s=0;s<num_rockets;++s){
        	for(size_t i=0; i<rockets[s].num_particles; ++i)
           		DrawCircle(		rockets[s].particles[i].pos.x, 
								rockets[s].particles[i].pos.y, 
								rockets[s].particles[i].rad, 
								rockets[s].particles[i].color);
		}
        DrawFPS(10.0, 10.0);
		EndDrawing();
    }
	
	free_rockets(rockets, num_rockets);
    CloseWindow();
    return EXIT_SUCCESS;
}

void update_rockets(rocket_t *rockets, size_t num_rockets, Vector2 screendim){
	for(size_t i=0;i<num_rockets;++i){
		 if(rockets[i].height==0.0f || !update_particles(rockets[i].particles, rockets[i].num_particles, rockets[i].exploded, rockets[i].pos, rockets[i].sin_d, rockets[i].cos_d)){
				rockets[i].pos=(Vector2) {.x=GetRandomValue(screendim.x/4, screendim.x/4*3), .y=screendim.y+1};
				float dir=GetRandomValue(155, 205)/180.0*PI;
				rockets[i].sin_d=sin(dir);
				rockets[i].cos_d=cos(dir);
				rockets[i].dir=(Vector2) {.x=-rockets[i].sin_d*2.0f, .y=rockets[i].cos_d*2.0f};
				rockets[i].exploded=0;
				rockets[i].height=(float) (screendim.y/10+GetRandomValue(0,2*screendim.y/3)); 
				update_particles(rockets[i].particles, rockets[i].num_particles, rockets[i].exploded, rockets[i].pos, rockets[i].sin_d, rockets[i].cos_d);
		}

				
		rockets[i].pos.x+=rockets[i].dir.x;
		rockets[i].pos.y+=rockets[i].dir.y;
		if(rockets[i].pos.y<=rockets[i].height)
				rockets[i].exploded=1;
	}
}

rocket_t *initialize_rockets(size_t num_rockets, size_t num_particles){
	rocket_t *rockets=malloc(sizeof(rocket_t)*num_rockets);
	if(rockets==NULL){
			fprintf(stderr, "unable to allocate space for %lu rockets!\n", num_rockets);
			return NULL;
	}
	memset(rockets, 0, sizeof(rocket_t)*num_rockets);
	
	for(size_t i=0; i<num_rockets;++i){
		if((rockets[i].particles=malloc(sizeof(particle_t)*num_particles))==NULL)
				goto fail;
		rockets[i].num_particles=num_particles;
		rockets[i].exploded=0;
		rockets[i].height=0.0f;
	}

	return rockets;
fail:
	fprintf(stderr, "Unable to allocate space for all particles!\n");
	free_rockets(rockets, num_rockets);
	return NULL;	
}

void free_rockets(rocket_t *rockets, size_t num_rockets){
	for(size_t i=0;i<num_rockets;++i)
			free(rockets[i].particles);
	free(rockets);
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
			p->opstep=(int8_t) ((x<0?-x:x)/(0.05f+10.0/31.0f)*20.0f);
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
