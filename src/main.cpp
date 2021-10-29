#ifndef APP_VERSION
# define APP_VERSION "0.0.0"
#endif
//#define SDL_MAIN_HANDLED//not needed in msvc,need to enable this from cmake, now is needed in msvc, wtf
//#define RC_INVOKED//this should't be done

#ifndef APP_VERSION
# define APP_VERSION "0.0.0"
#endif


#include <iostream>
#include <iomanip>
#include <sstream>

#include <SDL2/SDL.h>
//#include <SDL_opengl.h>
#include <glad/glad.h>


using namespace std;


SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;


int main(void)
{
    // initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        // if succeeded create our window
        g_pWindow = SDL_CreateWindow("Hello SDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN);
        // if the window creation succeeded create our renderer
        if(g_pWindow != 0)
        {
        g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
        }
    }
    else
    {
        return 1; // sdl could not initialize
    }
    // everything succeeded lets draw the window
    // set to black // This function expects Red, Green, Blue and
    // Alpha as color values
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    // clear the window to black
    SDL_RenderClear(g_pRenderer);
    // show the window
    SDL_RenderPresent(g_pRenderer);
    // set a delay before quitting
    SDL_Delay(5000);
    // clean up SDL
    SDL_Quit();
    
    return 0;
}