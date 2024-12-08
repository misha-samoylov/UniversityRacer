#include <stdlib.h>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _MSC_VER
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GL/glu.h>

#include "Exceptions.h"
#include "Utils.h"
#include "BaseApp.h"
#include "Game.h"

BaseApp *application = NULL;

void PrintGLVersion()
{
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor: %s\n", vendor);
	printf("GL Renderer: %s\n", renderer);
	printf("GL Version (string): %s\n", version);
	printf("GL Version (integer): %d.%d\n", major, minor);
	printf("GLSL Version: %s\n", glslVersion);
}

SDL_Window* Init(unsigned width, unsigned height, unsigned color, unsigned depth, unsigned stencil)
{
    // Set OpenGL attributes
    if (SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, color) < 0) throw SDL_Exception();
    if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth) < 0) throw SDL_Exception();
    if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil) < 0) throw SDL_Exception();
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) throw SDL_Exception();

	// Zapnout antialiasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    glEnable(GL_MULTISAMPLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    //SDL_GL_SetAttribute(
    //    SDL_GL_CONTEXT_PROFILE_MASK,
    //    SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_Window* screen = SDL_CreateWindow("University Racer X",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (screen == NULL)
        throw SDL_Exception();

    SDL_GLContext glcontext = SDL_GL_CreateContext(screen);
    // SDL_GL_DeleteContext(glcontext);
    // SDL_DestroyWindow(window);

	// Inicializace glew
	GLenum err = glewInit();
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	PrintGLVersion();

	application = new Game();
    application->onInit(screen);
    application->onWindowResized(width, height);

    return screen;	
}

void MainLoop(SDL_Window *window)
{
    // Window is not minimized
    //bool active = true;

    GameTime gameTime;
    gameTime.Init(SDL_GetTicks());

    FpsCounter fpsCounter;

    SDL_Event event;

    for(;;) // Infinite loop
    {
        //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);


        // Screen needs redraw
        //bool redraw = true;
		
		// aktualizovat FPS
		//updateFPS(SDL_GetTicks());
        gameTime.Update(SDL_GetTicks());
        fpsCounter.Update(gameTime);

        //char text[256];
        //sscanf(text, "University Racer - Diff: %d, FPS %d, fpscounter %d", gameTime, fpsCounter, ((Game *)application)->statsString());
        //sscanf(text, "University Racer - FPS %d, ", fpsCounter);
        //SDL_WM_SetCaption(text.str().c_str(), NULL);

        //SDL_SetWindowTitle(window, "OK");

        // Handle all waiting events
        //Handle events on queue
        while (SDL_PollEvent(&event) != 0)
        {
            
        
            // Call proper event handlers
            switch (event.type) {
                // Stop redraw when minimized
				//case SDL_ACTIVEEVENT:
                //    if(event.active.state == SDL_APPACTIVE)
                //        active = (event.active.gain != 0);
                //    break;

				// Set redraw flag
                //case SDL_VIDEOEXPOSE:
                //    redraw = true;
                //    break;
					
				// End main loop
				case SDL_QUIT :
                    return;

				// Pass UI events to the app
                case SDL_KEYDOWN :                    
                case SDL_KEYUP :                    
                case SDL_MOUSEMOTION :                    
                case SDL_MOUSEBUTTONDOWN :                    
                case SDL_MOUSEBUTTONUP :                    
                case SDL_WINDOWEVENT_RESIZED: //SDL_VIDEORESIZE :
					application->handleEvent(event);
					break;

				// Do nothing
                default :
                    break;
            }
        }

        application->onWindowRedraw(gameTime);

#ifdef _DEBUG
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::string error;

            switch (err)
            {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }

            printf("err %s \n", error.c_str());
        }
    }
#endif
}

int main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef _MSC_VER
	// hlida stav heapu pri kazde de/alokaci
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif
#endif

	try {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
            throw SDL_Exception();
        
        SDL_Window *window = Init(1024, 768, 24, 24, 8);

		MainLoop(window);

		delete application;

    } catch(std::exception & ex) {
        printf("ERROR : %s\n", ex.what());
		return 1;
    }

	SDL_Quit();

#ifdef _DEBUG
#ifdef _MSC_VER
	// vypise pripadne memory leaky
	_CrtDumpMemoryLeaks();
#endif
#endif

    return 0;
}
