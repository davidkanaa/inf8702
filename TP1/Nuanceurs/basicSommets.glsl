#version 430 core

// Déclarations des variables passées par tampons:
layout(location = 0) in vec3 tex;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 vp;

// Déclaration des variables uniformes :
uniform vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Déclaration des variables de sorties:
out vec4 fragmentColor;

void main () {
    gl_Position = projection * view * model * vec4(vp, 1);
	
    // Attribuer des valeurs aux variables de sorties:
    fragmentColor = color;
}