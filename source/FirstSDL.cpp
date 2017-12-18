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
	#include "glm.hpp"
	#include "GL/glew.h"
	#include "SDL2/SDL_mixer.h"
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_opengl.h"
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


// Shader sources
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


// Our SDL_Window ( just like with SDL2 without OpenGL)
SDL_Window *mainWindow;

// Our opengl context handle
SDL_GLContext mainContext;




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


	// Initialize SDL's subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Failed to init SDL\n";
		return false;
	}

    // Create our window centered at 512x512 resolution
	mainWindow = SDL_CreateWindow("OpenGL Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
1024, 768, SDL_WINDOW_OPENGL);

	// Check that everything worked out okay
	if (!mainWindow)
	{
		std::cout << "Unable to create window\n";
		return false;
	}


	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards would be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create our opengl context and attach it to our window
	mainContext = SDL_GL_CreateContext(mainWindow);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

	// turn GLEW experimental on
    glewExperimental= GL_TRUE;

    // check for GLEW init()
	GLenum err=glewInit();
	if(err!=GLEW_OK) {
	  // Problem: glewInit failed, something is seriously wrong.
	  cout << "glewInit failed: " << glewGetErrorString(err) << endl;
	  exit(1);
	}

//comment 2
    //set up background audio
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 640);
	Mix_Music *music = Mix_LoadMUS((dirAudio + "Haunted.ogg").c_str());
    Mix_PlayMusic(music, -1);



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





	// build and compile our shader program
	    // ------------------------------------
	    // vertex shader
	    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	    glCompileShader(vertexShader);
	    // check for shader compile errors
	    int success;
	    char infoLog[512];
	    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	    if (!success)
	    {
	        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	    }
	    // fragment shader
	    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	    glCompileShader(fragmentShader);
	    // check for shader compile errors
	    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	    if (!success)
	    {
	        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	    }
	    // link shaders
	    int shaderProgram = glCreateProgram();
	    glAttachShader(shaderProgram, vertexShader);
	    glAttachShader(shaderProgram, fragmentShader);
	    glLinkProgram(shaderProgram);
	    // check for linking errors
	    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	    if (!success) {
	        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	    }
	    glDeleteShader(vertexShader);
	    glDeleteShader(fragmentShader);

	    // set up vertex data (and buffer(s)) and configure vertex attributes
	    // ------------------------------------------------------------------
	    float vertices[] = {
	        -0.5f, -0.5f, 0.0f, // left
	         0.5f, -0.5f, 0.0f, // right
	         0.0f,  0.5f, 0.0f  // top
	    };

	    unsigned int VBO, VAO;
	    glGenVertexArrays(1, &VAO);
	    glGenBuffers(1, &VBO);
	    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	    glBindVertexArray(VAO);

	    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	    glEnableVertexAttribArray(0);

	    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	    glBindVertexArray(0);

	    // uncomment this call to draw in wireframe polygons.
	    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


























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



		  			// render
		  			        // ------
		  			        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		  			        glClear(GL_COLOR_BUFFER_BIT);

		  			        // draw our first triangle
		  			        glUseProgram(shaderProgram);
		  			        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		  			        glDrawArrays(GL_TRIANGLES, 0, 3);
		  			        // glBindVertexArray(0); // no need to unbind it every time


		  			        SDL_GL_SwapWindow(mainWindow);





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


	    // optional: de-allocate all resources once they've outlived their purpose:
	    // ------------------------------------------------------------------------
	    glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);

	SDL_GL_DeleteContext(mainContext);
	Mix_FreeMusic(music);

	SDL_Quit();

    return 0;
}

