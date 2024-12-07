#version 130

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
//in vec3 normal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 modelView;
uniform mat4 modelViewProjection;
uniform mat3 mv_inverse_transpose;

void main() {
	vec4 pos = vec4(position, 1.0);
	gl_Position = modelViewProjection * pos;	
}
