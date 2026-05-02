#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Globals.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib> // for rand and srand


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                w,
                                h,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();


    // Setup our Renderer
    m_renderer = new Renderer(w,h);    
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    if(m_renderer!=nullptr){
        delete m_renderer;
    }


    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}



//Loops forever!
void SDLGraphicsProgram::Loop(){

    // TODO: Understand this
    // Create our terrain
    g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
    g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
    // Create a node for our terrain 
    SceneNode* terrainNode;
    terrainNode = new SceneNode(g.GlobalTerrain);
    // Set our SceneTree up
    m_renderer->setRoot(terrainNode);

    // Set a default position for our camera
    m_renderer->GetCamera(0)->SetCameraEyePosition(125.0f,150.0f,500.0f);

    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 5.0f;

    // While application is running
    while(!quit){

        // For our terrain setup the identity transform each frame
        // TODO maybe move this
        terrainNode->GetLocalTransform().LoadIdentity();


        //Handle events on queue
        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
            }
            // Camera movement
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                m_renderer->GetCamera(0)->MouseLook(mouseX, mouseY);
            }
            switch(e.type){
                // Handle keyboard presses
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_a:
                            m_renderer->GetCamera(0)->MoveLeft(cameraSpeed);
                            break;
                        case SDLK_d:
                            m_renderer->GetCamera(0)->MoveRight(cameraSpeed);
                            break;
                        case SDLK_w:
                            m_renderer->GetCamera(0)->MoveForward(cameraSpeed);
                            break;
                        case SDLK_s:
                            m_renderer->GetCamera(0)->MoveBackward(cameraSpeed);
                            break;
                        case SDLK_q:
                            m_renderer->GetCamera(0)->MoveUp(cameraSpeed);
                            break;
                        case SDLK_e:
                            m_renderer->GetCamera(0)->MoveDown(cameraSpeed);
                            break;
                            // create a new random terrain
                        case SDLK_r:
                            g.seed = rand() % 1000;
                            g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
                            g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
                            // Create a node for our terrain
                            SceneNode* terrainNode;
                            terrainNode = new SceneNode(g.GlobalTerrain);
                            // Set our SceneTree up
                            m_renderer->setRoot(terrainNode);
                            break;

                            // decrease the terrain complexity
                        case SDLK_DOWN:
                            if (g.octaves > 0){
                                g.octaves = g.octaves - 1;
                                g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
                                g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
                                // Create a node for our terrain
                                SceneNode* terrainNode;
                                terrainNode = new SceneNode(g.GlobalTerrain);
                                // Set our SceneTree up
                                m_renderer->setRoot(terrainNode);
                            }
                            break;
                            // increase terrain complexity
                        case SDLK_UP:
                            if (g.octaves < 9){
                                g.octaves = g.octaves + 1;
                                g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
                                g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
                                // Create a node for our terrain
                                SceneNode* terrainNode;
                                terrainNode = new SceneNode(g.GlobalTerrain);
                                // Set our SceneTree up
                                m_renderer->setRoot(terrainNode);
                            }
                            break;

                            // Height modifications
                        case SDLK_EQUALS:
                            if (g.scale < 750) {
                                g.scale = g.scale + 20;
                                g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
                                g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
                                // Create a node for our terrain
                                SceneNode* terrainNode;
                                terrainNode = new SceneNode(g.GlobalTerrain);
                                // Set our SceneTree up
                                m_renderer->setRoot(terrainNode);
                            }
                            break;
                        case SDLK_MINUS:
                            if (g.scale > 50) {
                                g.scale = g.scale - 20;
                                g.GlobalTerrain = new Terrain(512,512,"./../../common/textures/terrain2.ppm");
                                g.GlobalTerrain->LoadTexture("./../../common/textures/colormap.ppm");
                                // Create a node for our terrain
                                SceneNode* terrainNode;
                                terrainNode = new SceneNode(g.GlobalTerrain);
                                // Set our SceneTree up
                                m_renderer->setRoot(terrainNode);
                            }
                            break;
                    }
                break;
            }
        } // End SDL_PollEvent loop.
		
        // Update our scene through our renderer
        m_renderer->Update();
        // Render our scene using our selected renderer
        m_renderer->Render();
        // Delay to slow things down just a bit!
        SDL_Delay(25);  // TODO: You can change this or implement a frame
                        // independent movement method if you like.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
