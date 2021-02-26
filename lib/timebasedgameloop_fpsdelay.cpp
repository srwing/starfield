#define TICK_INTERVAL    30

Uint32 TimeLeft(void)
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}


/* main game loop

    while ( game_running ) {
        UpdateGameState();
        SDL_Delay(TimeLeft());
    }