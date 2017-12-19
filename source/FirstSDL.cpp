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

	#include <shader_s.h>
	#include <stb_image.h>




	#define getcwd _getcwd

#elif __linux__
    // linux
	#include "glm.hpp"
	#include "GL/glew.h"
	#include "GL/glu.h"
	#include "SDL2/SDL_mixer.h"
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_opengl.h"
	#include <unistd.h>

	#include "shader_s.h"
	#include "stb_image.h"


#elif __APPLE__
    #include <glm.hpp>
	#include <SDL_mixer.h>
	#include <SDL2/SDL.h>
	#include <glew.h>
	#include <SDL2/SDL_opengl.h>
	#include <unistd.h>

	#include "shader_s.h"
	#include "stb_image.h"
#endif

using namespace std;
#include <stdio.h>
#include <iostream>



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





    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("4.2.texture.vs", "4.2.texture.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);

















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

		  	        // bind textures on corresponding texture units
		  	        glActiveTexture(GL_TEXTURE0);
		  	        glBindTexture(GL_TEXTURE_2D, texture1);
		  	        glActiveTexture(GL_TEXTURE1);
		  	        glBindTexture(GL_TEXTURE_2D, texture2);

		  	        // render container
		  	        ourShader.use();
		  	        glBindVertexArray(VAO);
		  	        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


		  								/*
		  									    //**************************** test
		  									    //transformVertexShaderSource
		  									    // fragment shader
		  									    	    int transformVertexShader = glCreateShader(GL_VERTEX_SHADER);
		  									    	    glShaderSource(transformVertexShader, 1, &transformVertexShaderSource, NULL);
		  									    	    glCompileShader(transformVertexShader);
		  									    	    // check for shader compile errors
		  									    	    glGetShaderiv(transformVertexShader, GL_COMPILE_STATUS, &success);
		  									    	    if (!success)
		  									    	    {
		  									    	        glGetShaderInfoLog(transformVertexShader, 512, NULL, infoLog);
		  									    	        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		  									    	    }
		  									    //**************************** test
		  								*/		} // button 0 checks IF ends
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
/*
	  	//***************************************************** menu screen
	    // optional: de-allocate all resources once they've outlived their purpose:
	    // ------------------------------------------------------------------------
	    glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
	    //***************************************************** menu screen
*/
	    glDeleteVertexArrays(1, &VAO);
	    glDeleteBuffers(1, &VBO);
	    glDeleteBuffers(1, &EBO);





	SDL_GL_DeleteContext(mainContext);
	Mix_FreeMusic(music);

	SDL_Quit();

    return 0;
}

