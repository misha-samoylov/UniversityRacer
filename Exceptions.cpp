#include "Exceptions.h"

const char* getGlErrorString(GLenum error)
{
#define ERROR(e) case e : return #e
switch (error)
{
    ERROR(GL_NO_ERROR);
    ERROR(GL_INVALID_ENUM);
    ERROR(GL_INVALID_VALUE);
    ERROR(GL_INVALID_OPERATION);
    ERROR(GL_INVALID_FRAMEBUFFER_OPERATION);
    ERROR(GL_OUT_OF_MEMORY);
default:
    return "UNKNOWN_GL_ERROR";
}
#undef ERROR
}
