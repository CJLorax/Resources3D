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

	//#include <shader_s.h>
#include <shader_m.h>
#include <stb_image.h>

#include <camera.h>




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

	//#include "shader_s.h"
#include "shader_m.h"
#include "stb_image.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#elif __APPLE__
    #include <glm.hpp>
	#include <SDL_mixer.h>
	#include <SDL2/SDL.h>
	#include <glew.h>
	#include <SDL2/SDL_opengl.h>
	#include <unistd.h>

	//#include "shader_s.h"
#include "shader_m.h"
#include "stb_image.h"

#include "camera.h"
#endif

using namespace std;
#include <stdio.h>
#include <iostream>



// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;



// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



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












	  // enter the program loop - See SDL_PollEvent

	  // ********** game loop ********
	  while(!quit){

		  switch(gameState){

		  	  case MENU:
		  	  {

		  		glDisable(GL_DEPTH_TEST);

		  	    // build and compile our shader zprogram
		  	    // ------------------------------------
		  	    Shader ourShader("4.2.texture.vs", "4.2.texture.fs");

		  	    // set up vertex data (and buffer(s)) and configure vertex attributes
		  	    // ------------------------------------------------------------------
		  	    float vertices[] = {

			  	    // positions          // colors           // texture coords
		  	    	0.75f, 0.55f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
					0.75f, 0.30f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
				   -0.75f, 0.30f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
				   -0.75f, 0.55f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left


		  	        // positions          // colors           // texture coords
		  	         0.75f, 0.05f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		  	         0.75f, -0.20f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		  	        -0.75f, -0.20f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		  	        -0.75f, 0.05f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left

		  	        // positions          // colors           // texture coords
		  	         0.75f, -0.45f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		  	         0.75f, -0.70f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		  	        -0.75f, -0.70f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		  	        -0.75f, -0.45f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left
		  	    };

		  	    /*unsigned int indices[] = {
		  	        0, 1, 3, // first triangle
		  	        1, 2, 3  // second triangle
		  	    };
		  	    */
		  	    unsigned int indices[] = {
		  	    		0,1,3, 2,1,3, 4,5,7, 6,5,7, 8,9,11, 10,9,11
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

		  		glDisable(GL_DEPTH_TEST);

		  	    // build and compile our shader zprogram
		  	    // ------	  	    // set texture filtering parameters
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
		  			        for (unsigned int i = 0; i < 10; i++)
		  			      	{
		  			        	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		  			      	}

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
			      // optional: de-allocate all resources once they've outlived their purpose:
			      // ------------------------------------------------------------------------
			      glDeleteVertexArrays(1, &VAO);
			      glDeleteBuffers(1, &VBO);
			      glDeleteBuffers(1, &EBO);
		  		  break;
		  	  }


		  	case INSTRUCTIONS:
		  	{
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
		  	}


		  	case SINGLE:
		  	{
		  		single = true;
		  		cout << "The GameState is SINGLE PLAYER GAME" << endl;
		  		cout << "Press the A button for Main Menu" << endl;
		  		cout << endl;


		  		// configure global opengl state
		  		    // -----------------------------
		  		    glEnable(GL_DEPTH_TEST);


				    // build and compile our shader zprogram
				    // ------------------------------------
				    Shader ourShader("7.2.camera.vs", "7.2.camera.fs");

				    // set up vertex data (and buffer(s)) and configure vertex attributes
				    // ------------------------------------------------------------------
				    float vertices[] = {
				        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
				         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
				         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

				        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
				        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

				        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
				         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

				        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
				        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
				    };
				    // world space positions of our cubes
				    glm::vec3 cubePositions[] = {
				        glm::vec3( 0.0f,  0.0f,  0.0f),
				        glm::vec3( 2.0f,  5.0f, -15.0f),
				        glm::vec3(-1.5f, -2.2f, -2.5f),
				        glm::vec3(-3.8f, -2.0f, -12.3f),
				        glm::vec3( 2.4f, -0.4f, -3.5f),
				        glm::vec3(-1.7f,  3.0f, -7.5f),
				        glm::vec3( 1.3f, -2.0f, -2.5f),
				        glm::vec3( 1.5f,  2.0f, -2.5f),
				        glm::vec3( 1.5f,  0.2f, -1.5f),
				        glm::vec3(-1.3f,  1.0f, -1.5f)
				    };
				    unsigned int VBO, VAO;
				    glGenVertexArrays(1, &VAO);
				    glGenBuffers(1, &VBO);

				    glBindVertexArray(VAO);

				    glBindBuffer(GL_ARRAY_BUFFER, VBO);
				    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				    // position attribute
				    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				    glEnableVertexAttribArray(0);
				    // texture coord attribute
				    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				    glEnableVertexAttribArray(1);


				    // load and create a texture
				    // -------------------------
				    unsigned int texture1, texture2;
				    // texture 1
				    // ---------
				    glGenTextures(1, &texture1);
				    glBindTexture(GL_TEXTURE_2D, texture1);
				    // set the texture wrapping parameters
				    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				    // set texture filtering parameters
				    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				    // load image, create texture and generate mipmaps
				    int width, height, nrChannels;
				    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
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
				    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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
				    ourShader.use();
				    ourShader.setInt("texture1", 0);
				    ourShader.setInt("texture2", 1);

				    // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
				    // -----------------------------------------------------------------------------------------------------------
				    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
				    ourShader.setMat4("projection", projection);





		  		while(single){

		  			 float cameraSpeed = 2.5 * deltaTime;
		  			//Set texture based on current keystate
		  			const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
		  			if( currentKeyStates[ SDL_SCANCODE_UP ] ) {
		  				cameraPos += cameraSpeed * cameraFront;
		  			} else if( currentKeyStates[ SDL_SCANCODE_DOWN ] ) {
		  				cameraPos -= cameraSpeed * cameraFront;
		  			} else if( currentKeyStates[ SDL_SCANCODE_LEFT ] ) {
		  				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		  			} else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] ) {
		  				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		  			}

		  	        // per-frame time logic
		  	        // --------------------
		  	        float currentFrame = SDL_GetTicks()/1000.0f;
		  	        deltaTime = currentFrame - lastFrame;
		  	        lastFrame = currentFrame;



		  	        // render
		  	        // ------
		  	        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		  	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		  	        // bind textures on corresponding texture units
		  	        glActiveTexture(GL_TEXTURE0);
		  	        glBindTexture(GL_TEXTURE_2D, texture1);
		  	        glActiveTexture(GL_TEXTURE1);
		  	        glBindTexture(GL_TEXTURE_2D, texture2);

		  	        // activate shader
		  	        ourShader.use();

		  	        // camera/view transformation
		  	        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		  	        ourShader.setMat4("view", view);

		  	        // render boxes
		  	        glBindVertexArray(VAO);
		  	        for (unsigned int i = 0; i < 10; i++)
		  	        {
		  	            // calculate the model matrix for each object and pass it to shader before drawing
		  	            glm::mat4 model;
		  	            model = glm::translate(model, cubePositions[i]);
		  	            float angle = 20.0f * i;
		  	            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		  	            ourShader.setMat4("model", model);

		  	            glDrawArrays(GL_TRIANGLES, 0, 36);
		  	        }

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
		  									single = false;
		  									gameState = MENU;
		  								}


		  						} // button 0 checks IF ends
		  						break;
		  				} // end button input checks SWITCH
		  			} // end SINGLE Poll Event
		  		} // end SINGLE screen
			    // optional: de-allocate all resources once they've outlived their purpose:
			    // ------------------------------------------------------------------------
			    glDeleteVertexArrays(1, &VAO);
			    glDeleteBuffers(1, &VBO);
		  		break;
		  	}


		  	case MULTI:
		  	{
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

		  	}



		  } // end gameState SWITCH

	  } // end game WHILE




	SDL_GL_DeleteContext(mainContext);
	Mix_FreeMusic(music);

	SDL_Quit();

    return 0;
}

