#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define APP_NAME              "Sin Wave"
#define DEFAULT_SCREEN_WIDTH  800
#define DEFAULT_SCREEN_HEIGHT 600

#define FPS  30
#define MSPF (1000.0f/FPS)

#define LOG( level, ... ) do {                           \
    fprintf( stderr, level );                            \
    fprintf( stderr, " " );                              \
    fprintf( stderr, "[%s:%i] : ", __FILE__, __LINE__ ); \
    fprintf( stderr, "%s : ", __FUNCTION__ );            \
    fprintf( stderr, __VA_ARGS__ );                      \
    fprintf( stderr, "\n" );                             \
} while(0)

#define ERROR( ... ) LOG( "ERROR", __VA_ARGS__ )
#define WARN( ... )  LOG( "WARN", __VA_ARGS__ )
#define INFO( ... )  LOG( "INFO", __VA_ARGS__ )
#define DEBUG( ... ) LOG( "DEBUG", __VA_ARGS__ )

#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))

struct state {
    SDL_Window   *window;
    SDL_Renderer *renderer;

    int width, height;
    int dir, winflags, quit;

    float freq, phase, amp;
};

void
usage ( const char *progname ) {
    fprintf( stdout, "usage: %s [options]\n", progname );
    fprintf( stdout, "\n" );
    fprintf( stdout, "options:\n" );
    fprintf( stdout, "\t-W\tscreen width\n" );
    fprintf( stdout, "\t-H\tscreen height\n" );
    fprintf( stdout, "\t-f\tfullscreen\n" );
    fprintf( stdout, "\t-h\tprint this help message\n" );
    exit(0);
}

void
parse_args ( struct state *state, int argc, char *argv[] ) {
    for ( int i = 1; i < argc; i++ ) {
        if ( strcmp( "-W", argv[i] ) == 0 ) {
            state->width = atoi(argv[++i]);
        } else if ( strcmp( "-H", argv[i] ) == 0 ) {
            state->height = atoi(argv[++i]);
        } else if ( strcmp( "-f", argv[i]) == 0 ) {
            state->winflags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else if ( strcmp( "-F", argv[i]) == 0 ) {
            state->freq = atof(argv[++i]);
        } else if ( strcmp( "-a", argv[i]) == 0 ) {
            state->amp = atof(argv[++i]);
        } else if ( strcmp( "-h", argv[i] ) == 0 ) {
            usage( argv[0] );
        } else {
            fprintf( stderr, "Invalid option \"%s\"\n", argv[i] );
        }
    }
}

int
init ( struct state *state, int argc, char *argv[] ) {
    srand(time(NULL));

    /* initialize defaults */
    memset( state, 0, sizeof(struct state) );
    state->width = DEFAULT_SCREEN_WIDTH;
    state->height = DEFAULT_SCREEN_HEIGHT;
    state->freq = M_PI;
    state->phase = ((float) rand()/ (float) RAND_MAX) * 2 * M_PI;
    state->amp = state->height/2;
    state->dir = 1;

    /* let user override certain properties */
    parse_args( state, argc, argv );
    
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        ERROR("%s", SDL_GetError());
        return 0;
    }

    int status = SDL_CreateWindowAndRenderer( 
            state->width, 
            state->height, 
            state->winflags, 
            &state->window, 
            &state->renderer
    );

    if ( status < 0 ) {
        ERROR("%s", SDL_GetError());
        return 0;
    }

    SDL_SetWindowTitle( state->window, APP_NAME );
    SDL_RenderSetLogicalSize( state->renderer, state->width, state->height );

    return 1;
}

void
handle_events ( struct state *state ) {
    SDL_Event e;

    while ( SDL_PollEvent(&e) ) {
        switch (e.type) {
            case SDL_QUIT:
                state->quit = 1;
                break;
            case SDL_KEYDOWN:
                if ( e.key.keysym.sym == SDLK_q ) { state->quit = 1; }
                break;
            default:
                break;
        }
    }
}

void
update ( struct state *state, int dt ) {
    /* 
    state->freq += dt * state->dir * 0.00005;
    
    if ( state->freq < 0.01f || state->freq > 0.1f ) { 
        state->dir = -state->dir;
    }
    */
}

void
render ( struct state *state ) {
    int hw = state->height/2;
    
    SDL_RenderClear(state->renderer);
    for ( int i = 0; i < state->width; i++ ) {
        float mod = sin(
            2 * M_PI * (state->freq * (float)i/state->width) + state->phase
        );
        int height = fabs(state->amp * mod);
        SDL_RenderDrawLine( state->renderer, i, hw - height, i, hw );
    }

    SDL_RenderPresent(state->renderer);
}

void
gameloop ( struct state *state ) {
    Uint32 start = 0;
    while ( !state->quit ) {
        Uint32 oldtime = start;
        start = SDL_GetTicks();
        handle_events(state);
        update(state, SDL_GetTicks() - oldtime);
        render(state);
        Sint32 diff = MSPF - SDL_GetTicks() + start;
        SDL_Delay(MAX(0, diff));
    }
}

void
quit ( struct state *state ) {
    if ( state->renderer ) { SDL_DestroyRenderer(state->renderer); }
    if ( state->window )   { SDL_DestroyWindow(state->window); }
    SDL_Quit();
}

int
main ( int argc, char *argv[] ) {
    struct state state;

    if ( init(&state, argc, argv) == 0 ) {
        quit(&state);
        return EXIT_FAILURE;
    }
    
    gameloop(&state);

    quit(&state);

    return EXIT_SUCCESS;
}
