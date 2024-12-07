#version 130

in vec2 texcoords;

uniform vec4 background; // barva pozadi
uniform sampler2D texture1;

void main() {	
	vec4 texcolor = texture(texture1, texcoords);

	if (texcolor != vec4(1.0f, 0.0f, 1.0f, 1.0f)) // klicovaci barva #ff00ff
		gl_FragColor = texcolor;
	else
		gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}
