#version 430 core

// Déclarations des variables passées par tampons (buffers):
layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 vc;

// Déclaration des variables uniformes :
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Déclaration des variables de sorties:
out vec3 fragmentColor;

void main () {
    gl_Position = projection * view * model * vec4(vp, 1);
	
    // Attribuer des valeurs aux variables de sorties:
    fragmentColor = vc;
}