// +------------------------------------------------------------+
// |                      University Racer                      |
// |         Projekt do PGR a GMU, FIT VUT v Brne, 2011         |
// +------------------------------------------------------------+
// |  Autori:  Tom� Kimer,  xkimer00@stud.fit.vutbr.cz         |
// |           Tom� Sychra, xsychr03@stud.fit.vutbr.cz         |
// |           David �abata, xsabat01@stud.fit.vutbr.cz         |
// +------------------------------------------------------------+

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <SDL.h>
#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Debug.h"
#include "Exceptions.h"
#include "GameTime.h"
#include "FpsCounter.h"

#ifndef M_PI
#define M_PI 3.14159f //265358979323846264338327950288419716939937510
#endif


#endif
