#version 430 core
struct Light
{
        vec3 Ambient; 
        vec3 Diffuse;
        vec3 Specular;
        vec4 Position;  // Si .w = 1.0 -> Direction de lumiere directionelle.
        vec3 SpotDir;
        float SpotExp;
        float SpotCutoff;
        vec3 Attenuation; //Constante, Lineraire, Quadratique
};

struct Mat
{
        vec4 Ambient; 
        vec4 Diffuse;
        vec4 Specular;
        vec4 Exponent;
        float Shininess;
};

layout(location = 0) in vec2 vt;
layout(location = 1) in vec3 vn;
layout(location = 2) in vec3 vp;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 MV_N;
uniform mat4 M;
uniform mat4 V;



// outputs
out vec2 fragTexCoord;

out vec3 offset;	// offset for light vectors

out vec3 normal;	// normal vector



void main (void)
{
    vec3 csPosition3;
    vec3 normal_cameraSpace;
    
    // Transformation de la position
    gl_Position = MVP * vec4(vp,1.0);
    vec4 csPosition = MV * vec4 (vp, 1.0);
    csPosition3 = (csPosition.xyz);

    //Normale en référentiel caméra:
    normal_cameraSpace = normalize(MV_N * vn);
    
    //Coordonée de texture:
    fragTexCoord = vt;  // Pass the tex coordinates to the fragment shader
    
    // Transformation de la position
    gl_Position = MVP * vec4(vp, 1.0);

	// Pass the offset vector for the light vectors to the fragment shader
	offset = csPosition3;

	//
	normal = normal_cameraSpace;
    
}
