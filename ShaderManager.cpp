#include "ShaderManager.h"

#define DEFAULT_PROGRAM "diffuse"
#define SHADERS_PATH "materials/"
#define TEXTURES_PATH "materials/textures/"

// init statickych promennych
ShaderManager::PROGRAMBINDING ShaderManager::currentProgram;

std::map<std::string, ShaderManager::PROGRAMBINDING> ShaderManager::programs = std::map<std::string, ShaderManager::PROGRAMBINDING>();
std::map<std::string, GLuint> ShaderManager::textures = std::map<std::string, GLuint>();

GLenum ShaderManager::activeTexture = GL_NONE;

std::vector<std::string> ShaderManager::shadersToLoad = std::vector<std::string>();
std::map<std::string, std::string> ShaderManager::shaderSubstitutions;

void ShaderManager::loadPrograms()
{
	for (std::vector<std::string>::iterator it = shadersToLoad.begin(); it != shadersToLoad.end(); it++)
	{
		if (!loadProgram(*it)) {
			std::cerr << "Warning: Shader for material '" << (*it) << "' not found" << std::endl;
		}
	}

	// fallback, musi byt dostupny vzdy
	loadDefaultProgram();
}

bool ShaderManager::loadProgram(std::string material)
{
	// pokud jiz program existuje, neni treba jej nacitat znovu
	if (programs.find(material) != programs.end())
		return true;

	PROGRAMBINDING mat;
	
	// VS
	std::string vsPath = SHADERS_PATH + material + ".vert";
	std::string vsSource;

	try { vsSource = loadFile(vsPath.c_str()); } 
	catch (String_Exception e) { return false; }

	GLuint vs = compileShader(GL_VERTEX_SHADER, vsSource.c_str());


	// FS
	std::string fsPath = SHADERS_PATH + material + ".frag";
	std::string fsSource;
	
	try { fsSource = loadFile(fsPath.c_str()); }
	catch (String_Exception e) { return false; }

	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource.c_str());


	// Program	
	mat.program = linkShader(2, vs, fs);
	
	// Vstupy
	mat.positionAttrib = glGetAttribLocation(mat.program, "position");
	mat.normalAttrib = glGetAttribLocation(mat.program, "normal");
	mat.tangentAttrib = glGetAttribLocation(mat.program, "tangent");
	mat.texposAttrib = glGetAttribLocation(mat.program, "texpos");

	mat.mViewUniform = glGetUniformLocation(mat.program, "view");
	mat.mModelViewUniform = glGetUniformLocation(mat.program, "modelView");
	mat.mModelViewProjectionUniform = glGetUniformLocation(mat.program, "modelViewProjection");	
	mat.mMVInverseTranspose = glGetUniformLocation(mat.program, "mv_inverse_transpose");

	mat.iEnabledLightsUniform = glGetUniformLocation(mat.program, "enabledLights");
	mat.vLightsUniform = glGetUniformLocation(mat.program, "lights");
	
	mat.matParams.ambient = glGetUniformLocation(mat.program, "material.ambient");
	mat.matParams.diffuse = glGetUniformLocation(mat.program, "material.diffuse");
	mat.matParams.specular = glGetUniformLocation(mat.program, "material.specular");
	mat.matParams.shininess = glGetUniformLocation(mat.program, "material.shininess");

	mat.bDrawAmbientUniform = glGetUniformLocation(mat.program, "paintAmbient");
	mat.bDrawDiffSpecUniform = glGetUniformLocation(mat.program, "paintDiffSpec");

	mat.bUseTextureUniform = glGetUniformLocation(mat.program, "useTexture");

	for (unsigned int i = 1; i <= 3; i++)
	{
		std::ostringstream uniformName;
		uniformName << "texture" << i;
		mat.textureUniforms.push_back( glGetUniformLocation(mat.program, uniformName.str().c_str()) );
	}

	programs[material] = mat;

	return true;
}

void ShaderManager::useMaterialParams(ShaderManager::MATERIALPARAMS params)
{
	PROGRAMBINDING mat = getCurrentProgram();
	
	// nastavi hodnoty materialu
	glUniform4fv(mat.matParams.ambient, 1, glm::value_ptr(params.ambient));
	glUniform4fv(mat.matParams.diffuse, 1, glm::value_ptr(params.diffuse));
	glUniform4fv(mat.matParams.specular, 1, glm::value_ptr(params.specular));
	glUniform1i(mat.matParams.shininess, params.shininess);
	
	// aktivovat pozadovane textury
	GLenum textureEnums[] = { GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5 };
	for (unsigned int i = 0; i < params.textures.size(); i++)
	{
		if (activeTexture != textureEnums[i]) {
			glActiveTexture(textureEnums[i]);
			activeTexture = textureEnums[i];
		}

		glBindTexture(GL_TEXTURE_2D, params.textures[i]);
		glUniform1i(mat.textureUniforms[i], i);		
	}

	// natavit priznak zda jsme predali nejakou texturu
	glUniform1i(mat.bUseTextureUniform, (params.textures.size() > 0));
}

ShaderManager::PROGRAMBINDING ShaderManager::useProgram(std::string program)
{	
	// overit substituci shaderu
	std::map<std::string, std::string>::iterator substIt = shaderSubstitutions.find(program);
	if (substIt != shaderSubstitutions.end())
		program = (*substIt).second;


	std::map<std::string, PROGRAMBINDING>::iterator el = programs.find(program);
	
	// fallback shaderu
	if (el == programs.end())
		el = programs.find(DEFAULT_PROGRAM);	

	PROGRAMBINDING mat = (*el).second;

	// nastavi aktualni shader v manageru
	currentProgram = mat;

	// a v GL
	glUseProgram(mat.program);
	
	return mat;	
}

ShaderManager::PROGRAMBINDING ShaderManager::getCurrentProgram()
{
	return currentProgram;
}

GLuint ShaderManager::loadTexture(std::string path)
{
	SDL_Surface * surface = SDL_LoadBMP(path.c_str());

	if(surface == NULL)
		throw SDL_Exception();

	GLuint handle;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automaticke mipmapy pro starsi gl
	glGenerateMipmap(GL_TEXTURE_2D);
	SurfaceImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return handle;
}

void ShaderManager::loadDefaultProgram()
{
	if (!loadProgram(DEFAULT_PROGRAM))
		throw std::runtime_error("Default material shader not found");
}

ShaderManager::MATERIALPARAMS ShaderManager::getDefaultMaterial()
{
	MATERIALPARAMS params;
	params.ambient = glm::vec4(0.5, 0.5, 0.5, 1);
	params.diffuse = glm::vec4(0.3, 0.3, 0.3, 1);
	params.specular = glm::vec4(0.8, 0.8, 0.8, 1);
	params.shininess = 10;

	return params;
}

std::string ShaderManager::getTexturesPath()
{
	return TEXTURES_PATH;
}

// Nacte obrazek do textury
void ShaderManager::SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface)
{
	// nejdriv vertikalne prevratit, at ma obrazek stejne souradnice jako GL
	verticalSurfaceFlip(surface);

    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    if(     (surface->format->Rmask == 0xff0000) &&
            (surface->format->Gmask == 0xff00) &&
            (surface->format->Bmask == 0xff) &&
            (surface->format->Amask == 0))
    {
        glTexImage2D(target, level, internalformat, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else if((surface->format->Rmask == 0xff) &&
            (surface->format->Gmask == 0xff00) &&
            (surface->format->Bmask == 0xff0000) &&
            (surface->format->Amask == 0))
    {
        glTexImage2D(target, level, internalformat, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else
    {
        throw std::runtime_error("unsupported surface format");
    }
}

// Load whole file and return it as std::string
std::string ShaderManager::loadFile(const char * const file)
{
    std::ifstream stream(file);
	if(stream.fail()) throw String_Exception(std::string("Can't open \'") + file + "\'");
    return std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
}

// Info log contains errors and warnings from shader compilation
std::string ShaderManager::getShaderInfoLog(const GLuint shader)
{
	//GLint status;
	//char err_buf[512];
	//glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	//if (status != GL_TRUE) {
	//	glGetShaderInfoLog(shader, sizeof(err_buf), NULL, err_buf);
	//	err_buf[sizeof(err_buf) - 1] = '\0';
	//	printf("shader compilation failed: %s\n", err_buf);
	//}

	std::string log = "";
   // int length;
    //glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    //std::string log(length, ' ');
    // glGetShaderInfoLog(shader, length, NULL, &log[0]);
    return log;
}

// Info log contains errors and warnings from shader linking
std::string ShaderManager::getProgramInfoLog(const GLuint program)
{
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::string log(length, ' ');
    // glGetProgramInfoLog(program, length, NULL, &log[0]);
    return log;
}

GLuint ShaderManager::compileShader(const GLenum type, const char * source)
{
    GLuint shader = glCreateShader(type);
    if(shader == 0) throw GL_Exception("glCreateShader failed");

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

	// vypisovat jenom pokud nastala chyba
	std::string log = getShaderInfoLog(shader);
	if (log.find("was successfully compiled") == std::string::npos)
		std::cout << "Compile: " << log << std::endl;

    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) throw std::runtime_error("shader compilation failed");

    return shader;
}

GLuint ShaderManager::linkShader(size_t count, ...)
{
    // Create program object
    GLuint program = glCreateProgram();
    if(program == 0) throw GL_Exception("glCreateProgram failed");

    // Attach arguments
    va_list args;
    va_start(args, count);
    for(size_t i = 0; i < count; ++i)
    {
        glAttachShader(program, va_arg(args, GLuint));
    }
    va_end(args);

    // Link program and check for errors
    glLinkProgram(program);
	
	// vypisovta jen pokud nastala chyba
	std::string log = getProgramInfoLog(program);
	if (log.find("Error") != std::string::npos || log.find("error") != std::string::npos || log.find("Warning") != std::string::npos || log.find("warning") != std::string::npos)
		std::cout << "Link: " << log << std::endl;

    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE) throw std::runtime_error("shader linking failed");

    return program;
}

/*
 * Kod z webu pro horizontalni flip SDL_surface, resp.textury
 */

/*
 * http://www.libsdl.org/docs/html/guidevideo.html
 */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}	

/*
 * http://www.libsdl.org/docs/html/guidevideo.html
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void ShaderManager::verticalSurfaceFlip(SDL_Surface*& image)
{
	// create a copy of the image    
	SDL_Surface* flipped_image = SDL_CreateRGBSurface( SDL_SWSURFACE, image->w, image->h, image->format->BitsPerPixel,
		image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask );
	
	if (flipped_image == NULL) {
		printf("flipped_image = NULL \n");
	}

	// loop through pixels    
	for ( int y=0; y<image->h; y++ ) {
		for ( int x=0; x<image->w; x++ ) {            
			// copy pixels, but reverse the y pixels!
			putpixel( flipped_image, x, y, getpixel(image, x, image->h - y -1) );        
		}    
	}    
	// free original and assign flipped to it    
	SDL_FreeSurface( image );    
	image = flipped_image;
}
