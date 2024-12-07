// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#include "main.h"

#ifdef _DEBUG
	#define new MYDEBUG_NEW
#endif

using namespace std;

// Pointer to active application instance, which will recieve and handle SDL events
BaseApp* application = NULL;

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

SDL_Surface * init(unsigned width, unsigned height, unsigned color, unsigned depth, unsigned stencil)
{
    // Set OpenGL attributes
    if(SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, color) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil) < 0) throw SDL_Exception();
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) throw SDL_Exception();

	// Zapnout antialiasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    // Create window
    SDL_Surface * screen = SDL_SetVideoMode(width, height, color, SDL_OPENGL | SDL_RESIZABLE);
    if(screen == NULL) throw SDL_Exception();
	
	SDL_WM_SetCaption("University Racer", "University Racer");

#ifndef USE_GLEE
	// Inicializace glew	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	PrintGLVersion();

#endif
	// Create the Game and set as current application
	application = new Game;

    // Call init code
    application->onInit();
    application->onWindowResized(width, height);

    return screen;	
}




// Simple main loop
void mainLoop()
{
    // Window is not minimized
    bool active = true;

    GameTime gameTime;
    gameTime.Init(SDL_GetTicks());
    FpsCounter fpsCounter;

    for(;;) // Infinite loop
    {
        SDL_Event event;

        // Wait for event
        if(SDL_WaitEvent(&event) == 0) throw SDL_Exception();

        // Screen needs redraw
        bool redraw = false;
		
		// aktualizovat FPS
		//updateFPS(SDL_GetTicks());
        gameTime.Update(SDL_GetTicks());
        fpsCounter.Update(gameTime);
		ostringstream text;
        text << "University Racer - Diff:" << gameTime << ", FPS - " << fpsCounter << "; " << "GPU Mem: " << ((Game *)application)->statsString();
        SDL_WM_SetCaption(text.str().c_str(), NULL);

        // Handle all waiting events
        do
        {
            // Call proper event handlers
            switch(event.type)
            {
                // Stop redraw when minimized
				case SDL_ACTIVEEVENT :
                    if(event.active.state == SDL_APPACTIVE)
                        active = (event.active.gain != 0);
                    break;

				// Set redraw flag
                case SDL_VIDEOEXPOSE :
                    redraw = true;
                    break;
					
				// End main loop
				case SDL_QUIT :
                    return;

				// Pass UI events to the app
                case SDL_KEYDOWN :                    
                case SDL_KEYUP :                    
                case SDL_MOUSEMOTION :                    
                case SDL_MOUSEBUTTONDOWN :                    
                case SDL_MOUSEBUTTONUP :                    
                case SDL_VIDEORESIZE :
					application->handleEvent(event);
					break;

				// Do nothing
                default :
                    break;
            }
        } while(SDL_PollEvent(&event) == 1);

        // Optionally redraw window
        if (active && redraw) application->onWindowRedraw(gameTime);

		// immediately fire redraw event
		SDL_Event nextFrameEvent;
		nextFrameEvent.type = SDL_VIDEOEXPOSE;
		if(SDL_PushEvent(&nextFrameEvent) < 0) throw SDL_Exception();
    }
}



int main(int /*argc*/, char ** /*argv*/) 
{

#ifdef _DEBUG
#ifdef _MSC_VER
	// hlida stav heapu pri kazde de/alokaci
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
#endif
#endif

	try {
        // Init SDL - only video subsystem will be used
        if(SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();
        
		// initialize application
        init(800, 600, 24, 24, 8);

		// start the main loop
		mainLoop();

		// cleanup
		delete application;

    } catch(exception & ex) {
        cout << "ERROR : " << ex.what() << endl;        
		system("pause");
		return EXIT_FAILURE;
    }

	SDL_Quit();

#ifdef _DEBUG
#ifdef _MSC_VER
	// vypise pripadne memory leaky
	_CrtDumpMemoryLeaks();
#endif
#endif

    return EXIT_SUCCESS;
}


const char * getGlErrorString(GLenum error)
{
#define ERROR(e) case e : return #e
    switch(error)
    {
        ERROR(GL_NO_ERROR);
        ERROR(GL_INVALID_ENUM);
        ERROR(GL_INVALID_VALUE);
        ERROR(GL_INVALID_OPERATION);
        ERROR(GL_INVALID_FRAMEBUFFER_OPERATION);
        ERROR(GL_OUT_OF_MEMORY);
    default : 
        return "UNKNOWN_GL_ERROR";
    }
#undef ERROR
}