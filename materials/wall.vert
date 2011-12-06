#version 130

#define MAX_LIGHTS 4

/////////////////////////////////////////////////
// Musi zustat stejne
/////////////////////////////////////////////////
in vec3 position; 
in vec3 normal;
in vec2 texpos;
in vec3 tangent;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat3 mv_inverse_transpose;

uniform vec3 eye;
uniform vec3 sight;

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};
uniform Material material;

uniform vec4 lights[30]; // kazde tri vektory odpovidaji jednomu svetlu: pozice, difuzni, ambientni slozka; max 10 svetel
uniform int enabledLights; // pocet pouzitych svetel (naplnenych do lights)

out vec2 t;

/////////////////////////////////////////////////

//tangent space
out vec3 tanCam;
out vec3 tanLightDir[MAX_LIGHTS];

out vec4 color;

void main() {
	vec3 eyeLightPos[MAX_LIGHTS];
	vec3 eyeNormal; // normala zkomaneho bodu v prostoru OKA
	vec3 eyePosition; // pozice zkoumaneho bodu v prostoru OKA

	//prevod texturovacich souradnic
	t = texpos;

	vec4 pos = vec4(position, 1.0);
	mat4 mv = view * model;
	mat4 mvp = projection * view * model;
	gl_Position = mvp * pos;

	//transformace normaly do prostoru OKA
	eyeNormal = normalize(mv_inverse_transpose * normal);
	
	//transformace zkoumaneho bodu do prostoru OKA
	vec4 eyePosition4 = mv * pos;
	eyePosition = eyePosition4.xyz / eyePosition4.w;
	
	// Transformujeme pozice svetel do eyespace				
	for(int i=0; i < MAX_LIGHTS; i++) {
		// predpokladame enabledLights > 0
		vec4 lightPosition = lights[i * 3 + 0]; // 0 == prvni hodnota prvniho svetla == pozice

		//transformace svetla do eyespace (pravdepodobne je ve worldspace)
		//out LightPosition
		vec4 lightPos4 = view * lightPosition;
		eyeLightPos[i] = lightPos4.xyz / lightPos4.w ;
	}
	///////////////////////////TANGENT SPACE MATRIX///////////////////////////////
	//priprave pro PARALLAX MAPPING
	//vypocet tangent, binormal

    //vec3 eyeTan = mv_inverse_transpose * tangent; // nejak tak to bude vypadat cca
	vec3 eyeTan = vec3(1.0,0.0,0.0); // TODO provizorni, chce to jine!
	vec3 binormal = normalize(cross(eyeNormal, eyeTan ));

	//vytvorime matici tbnMatrix, ktera bude transformovat body do TangentSpace
	mat3 tbnMatrix = mat3(eyeTan, binormal, eyeNormal);

	/////////////////////////TRANSFORM TO TANGENT SPACE/////////////////////
	vec3 eyeCam = -eyePosition;
	//prevedeme kameru, pozici svetla, plosku do tangent space
	tanCam = eyeCam * tbnMatrix;

	for( int i = 0; i < MAX_LIGHTS; i++) {
		vec3 lightDir = eyeLightPos[i] - eyePosition;
		tanLightDir[i] = lightDir * tbnMatrix;
	}

	color = vec4(texpos,texpos);
	
}