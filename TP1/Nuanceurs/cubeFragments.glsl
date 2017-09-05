#version 430 core

// Déclaration des variables passées par le nuanceur de sommets:
in vec3 fragmentColor;

// Déclaration des variables de sortie
out vec4 color;

void main () {

    // À changer:
	// color = vec4(0.5, 0.5, 0.5, 1.0);
	color = vec4(fragmentColor, 1);
}