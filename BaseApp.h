#ifndef BASEAPP_H
#define BASEAPP_H

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>

#include "Exceptions.h"
#include "Utils.h"
#include "ModelContainer.h"
#include "Scene.h"
#include "ShaderManager.h"
#include "Camera.h"


class BaseApp {

public:
	BaseApp();
	virtual ~BaseApp();

	Camera* const getCamera();		

	virtual void handleEvent(SDL_Event);
		
	virtual void onInit(SDL_Window*);
	virtual void onWindowRedraw(const GameTime & gameTime);		

	virtual void onWindowResized(int w, int h);
	virtual void onKeyDown(SDL_Keycode key, Uint16 /*mod*/);
	virtual void onKeyUp(SDL_Keycode /*key*/, Uint16 /*mod*/);
	virtual void onMouseMove(unsigned /*x*/, unsigned /*y*/, int xrel, int yrel, Uint8 buttons);
	virtual void onMouseDown(Uint8 button, unsigned /*x*/, unsigned /*y*/);
	virtual void onMouseUp(Uint8 /*button*/, unsigned /*x*/, unsigned /*y*/);

	// Vraci pomer stran okna aplikace
	inline double getWindowAspectRatio()
	{
		return static_cast<double>(windowWidth) / windowHeight;
	}


protected:
	// rozmery viewportu
	int windowWidth, windowHeight;

	Camera camera;

	// seznam aktualne stisknutych klaves
	std::list<SDL_Keycode> activeKeys;

	// Send quit event
	inline void quit()
	{
		SDL_Event event;
		event.type = SDL_QUIT;

		if (SDL_PushEvent(&event) < 0)
			throw SDL_Exception();
	}
};

#endif
