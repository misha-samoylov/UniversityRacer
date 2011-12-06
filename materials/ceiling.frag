#version 130

#define MAX_LIGHTS 4

// @LOAD materials/textures/ceiling.bmp
uniform sampler2D tex;

// @LOAD materials/textures/ceiling_bump.bmp
uniform sampler2D texNormal;

// @LOAD materials/textures/ceiling.bmp
uniform sampler2D texHeight;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform Material material;

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

//tangent space
in vec3 tanCam;
in vec3 tanLightDir[MAX_LIGHTS];

//texture coordinates
in vec2 t;

//debug color
in vec4 color;

void main() {
	
	//nove koordinaty po posunuti v parallax mappingu
	vec2 newTexCoord;

	vec4 finalColor = vec4(0.5,0.5,0.5,1.0);

	float scaleCoord = 5;

	//////////////////////////////OSVETLENI////////////////////////////////
	//Nastaveni fyzikalnich konstant pro slabnuti svetla se vzdalenosti
	float attenuation, distance, radius; 
	radius = 0.6;
	//pro Range 100 - zdroj : http://www.ogre3d.org/tikiwiki/-Point+Light+Attenuation
	float constantAtt = 1.0;
	float linearAtt = 0.0014;
	float quadraticAtt = 0.00007;
	vec4 diffuseF;
	for(int i = 0; i < MAX_LIGHTS; i++) { 
		distance = length(tanLightDir[i] / radius);	
		attenuation = 1.0 / (constantAtt + linearAtt * distance +
										   quadraticAtt * distance * distance);
									   
		//spocitame odraz (difuzni slozku)	
		vec3 nMap = normalize(texture(texNormal, t * scaleCoord).rgb * 2.0 - 1.0);
					   	
		vec3 N = normalize(nMap); //normala plosky dle textury
		vec3 L = normalize(tanLightDir[i]); //paprsek svetla v tangentovem prostoru


		//difuzni slozka
		float diffuse = max(dot(N,L),0.0);
		
		diffuseF =  material.diffuse * lights[i * 3 + 1]; // * material.diffuse;
		finalColor += attenuation * diffuse * diffuseF;
	}
	

	//gl_FragColor = color;
	//gl_FragColor = vec4(t,0.0,1.0);		
	gl_FragColor = texture(tex, t * scaleCoord) * finalColor;
}