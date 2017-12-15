// Example program:
// Using SDL2 to create an application window

#ifdef _WIN32 || _WIN64
    // Windows (x64 and x86)
	#include <windows.h>
	#include <GL/glew.h>
	#include <glm.hpp>
	#include <GL/gl.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>
	#include <SDL2/SDL_mixer.h>
	#include <direct.h>
	#define getcwd _getcwd

#elif __linux__
    // linux
	#include "GL/glew.h"
	#include "SDL2/SDL.h"
	#include <unistd.h>

#elif __APPLE__
    #include <glm.hpp>
	#include <SDL_mixer.h>
	#include <SDL2/SDL.h>

	#include <glew.h>
	#include <SDL2/SDL_opengl.h>
	#include <unistd.h>
#endif

using namespace std;
#include <stdio.h>
#include <iostream>




int main(int argc, char* argv[]) {

#ifdef _WIN32 || _WIN64
    // Windows (x64 and x86)
	// get current working directory
	string s_cwd(getcwd(NULL,0));
	// string to link to the content folders
	string dirGraphics = s_cwd + "\\Resources3D\\graphics\\";
	string dirAudio = s_cwd + "\\Resources3D\\audio\\";
	string dirModels = s_cwd + "\\Resources3D\\models\\";


#elif __linux__
    // linux
	// get current working directory
	string s_cwd(getcwd(NULL,0));
	// string to link to the content folders
	string dirGraphics = s_cwd + "/Resources3D/graphics/";
	string dirAudio = s_cwd + "/Resources3D/audio/";
	string dirModels = s_cwd + "/Resources3D/models/";

#elif __APPLE__
	// apple
	// get current working directory
	string s_cwd(getcwd(NULL,0));
	// string to link to the content folders
	string dirGraphics = s_cwd + "/Resources3D/graphics/";
	string dirAudio = s_cwd + "/Resources3D/audio/";
	string dirModels = s_cwd + "/Resources3D/models/";


#endif

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Failed to init SDL\n");
       exit(1);
  }

cout << dirAudio << endl;

	// init() all SDL headers
	SDL_Init(SDL_INIT_EVERYTHING);

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

   //set up background audio
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 640);
	Mix_Music *music = Mix_LoadMUS((dirAudio + "Haunted.wav").c_str());
	//(dirAudio + "Haunted.wav").c_str()
    Mix_PlayMusic(music, 1);

	// turn GLEW experimental on
    glewExperimental=true;

    // check for GLEW init()
	  GLenum err=glewInit();
	  if(err!=GLEW_OK) {
	    // Problem: glewInit failed, something is seriously wrong.
	    cout << "glewInit failed: " << glewGetErrorString(err) << endl;
	    exit(1);
	  }


	  //set up a game controller
	  SDL_GameController* gGameController = NULL;

	  //open the game controller
	  gGameController = SDL_GameControllerOpen(0);

	  // turn on controller events
	  SDL_GameControllerEventState(SDL_ENABLE);

	  //SDL event to handle input
	  SDL_Event event;

	  // set up enum to handle game states
	  enum GameState {MENU, INSTRUCTIONS, SINGLE, MULTI, SINGLEWIN, SINGLELOSE, MULTIWIN, MULTILOSE};

	  //set enum to track where we are
	  GameState gameState = MENU;

	  // bool vars to track movement through the individual states
	  bool menu, instructions, single, multi, singlewin, singlelose, multiwin, multilose, quit = false;

	  // enter the program loop - See SDL_PollEvent

	  // ********** game loop ********
	  while(!quit){

		  switch(gameState){

		  	  case MENU:

		  		  menu = true;
		  		  cout << "The GameState is MENU" << endl;
		  		  cout << "Press the A button for Instructions" << endl;
		  		  cout << "Press the B button for Single Player Mode" << endl;
		  		  cout << "Press the X button for Multi-Player Mode" << endl;
		  		  cout << "Press the Y button to Quit" << endl;
		  		  cout << endl;

		  		  while(menu){

		  			  // check for input
		  			  if(SDL_PollEvent(&event)){
		  				  // see if the player closes the window
		  				  if(event.type == SDL_QUIT){
		  					  quit = true;
		  					  menu = false;
		  					  break;
		  				  } // end SDL Quit check

		  				  //test for controller input
		  				  switch(event.type){

		  				  case SDL_CONTROLLERBUTTONDOWN:

								  // checks to see if this is controller 0
								  if(event.cdevice.which == 0){

									  // if A button
									  if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
										  menu = false;
										  gameState = INSTRUCTIONS;
									  }

									  // if B button
									  if(event.cbutton.button == SDL_CONTROLLER_BUTTON_B){
										  menu = false;
										  gameState = SINGLE;
									  }

									  // if X button
									  if(event.cbutton.button == SDL_CONTROLLER_BUTTON_X){
										  menu = false;
										  gameState = MULTI;
									  }

									  // if Y button
									  if(event.cbutton.button == SDL_CONTROLLER_BUTTON_Y){
										  menu = false;
										  quit = true;
									  }

								  } // button 0 checks IF ends
								  break;

							} // end button input checks SWITCH
		  			  } // end MENU Poll Event
		  		  } // end MENU screen
		  		  break;


		  	case INSTRUCTIONS:

		  		instructions = true;
		  		cout << "The GameState is INSTRUCTIONS" << endl;
		  		cout << "Press the A button for Main Menu" << endl;
		  		cout << endl;

		  		while(instructions){

		  			// check for input
		  			if(SDL_PollEvent(&event)){
		  				// see if the player closes the window
		  				if(event.type == SDL_QUIT){
		  					quit = true;
		  					menu = false;
		  					break;
		  				} // end SDL Quit check

		  				//test for controller input
		  				switch(event.type){

		  					case SDL_CONTROLLERBUTTONDOWN:

		  						// checks to see if this is controller 0
		  						if(event.cdevice.which == 0){

		  							// if A button
		  								if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
		  									instructions = false;
		  									gameState = MENU;
		  								}


		  						} // button 0 checks IF ends
		  						break;
		  				} // end button input checks SWITCH
		  			} // end INSTRUCTIONS Poll Event
		  		} // end INSTRUCTIONS screen
		  		break;



		  	case SINGLE:
		  		single = true;
		  		cout << "The GameState is SINGLE PLAYER GAME" << endl;
		  		cout << "Press the A button for Main Menu" << endl;
		  		cout << endl;

		  		while(single){

		  			// check for input
		  			if(SDL_PollEvent(&event)){
		  				// see if the player closes the window
		  				if(event.type == SDL_QUIT){
		  					quit = true;
		  					menu = false;
		  					break;
		  				} // end SDL Quit check

		  				//test for controller input
		  				switch(event.type){

		  					case SDL_CONTROLLERBUTTONDOWN:

		  						// checks to see if this is controller 0
		  						if(event.cdevice.which == 0){

		  							// if A button
		  								if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
		  									single = false;
		  									gameState = MENU;
		  								}


		  						} // button 0 checks IF ends
		  						break;
		  				} // end button input checks SWITCH
		  			} // end SINGLE Poll Event
		  		} // end SINGLE screen
		  		break;



		  	case MULTI:
		  		multi = true;
		  		cout << "The GameState is MULTI-PLAYER GAME" << endl;
		  		cout << "Press the A button for Main Menu" << endl;
		  		cout << endl;

		  		while(multi){

		  			// check for input
		  			if(SDL_PollEvent(&event)){
		  				// see if the player closes the window
		  				if(event.type == SDL_QUIT){
		  					quit = true;
		  					menu = false;
		  					break;
		  				} // end SDL Quit check

		  				//test for controller input
		  				switch(event.type){

		  					case SDL_CONTROLLERBUTTONDOWN:

		  						// checks to see if this is controller 0
		  						if(event.cdevice.which == 0){

		  							// if A button
		  								if(event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
		  									multi = false;
		  									gameState = MENU;
		  								}


		  						} // button 0 checks IF ends
		  						break;
		  				} // end button input checks SWITCH
		  			} // end MULTI Poll Event
		  		} // end MULTI screen
		  		break;





		  } // end gameState SWITCH

	  } // end game WHILE





	SDL_GL_DeleteContext(context);
	Mix_FreeMusic(music);

	SDL_Quit();

    return 0;
}

