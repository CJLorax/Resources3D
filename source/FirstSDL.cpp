// Example program:
// Using SDL2 to create an application window

#ifdef _WIN32 // note the underscore: without it, it's not msdn official!
    // Windows (x64 and x86)
	#include <windows.h>
	#include <GL/glew.h>
	#include <glm.hpp>
	#include <GL/gl.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>


#elif __linux__
    // linux
	#include "GL/glew.h"
	#include "SDL2/SDL.h"

#elif __APPLE__
    #include <glm.hpp>
	#include <SDL2/SDL.h>
	#include <glew.h>
	#include <SDL2/SDL_opengl.h>
#endif

using namespace std;
#include <stdio.h>
#include <iostream>




int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	  glewExperimental=true;
	  GLenum err=glewInit();
	  if(err!=GLEW_OK) {
	    // Problem: glewInit failed, something is seriously wrong.
	    cout << "glewInit failed: " << glewGetErrorString(err) << endl;
	    exit(1);
	  }

	  /* This makes our buffer swap syncronized with the monitor's vertical refresh */
	    SDL_GL_SetSwapInterval(1);

	    /* Clear our buffer with a red background */
	    glClearColor ( 1.0, 0.0, 0.0, 1.0 );
	    glClear ( GL_COLOR_BUFFER_BIT );
	    /* Swap our back buffer to the front */
	    SDL_GL_SwapWindow(window);
	    /* Wait 2 seconds */
	    SDL_Delay(2000);

	    /* Same as above, but green */
	    glClearColor ( 0.0, 1.0, 0.0, 1.0 );
	    glClear ( GL_COLOR_BUFFER_BIT );
	    SDL_GL_SwapWindow(window);
	    SDL_Delay(2000);

	    /* Same as above, but blue */
	    glClearColor ( 0.0, 0.0, 1.0, 1.0 );
	    glClear ( GL_COLOR_BUFFER_BIT );
	    SDL_GL_SwapWindow(window);

    SDL_Delay(2000);

	SDL_GL_DeleteContext(context);

	SDL_Quit();

    return 0;
}

