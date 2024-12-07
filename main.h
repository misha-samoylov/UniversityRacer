// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#include <stdlib.h>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _MSC_VER
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include "Debug.h"
#endif

// Libraries ///////////////////////////////////////////

#include <SDL.h>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include <GL/glu.h>

////////////////////////////////////////////////////////

#include "Exceptions.h"
#include "Utils.h"
#include "BaseApp.h"
#include "Game.h"