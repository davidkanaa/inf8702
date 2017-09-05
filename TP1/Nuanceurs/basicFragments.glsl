#version 430 core

// Déclaration des variables passées par le nuanceur de sommets:
in vec4 fragmentColor;

// Déclaration des variables de sortie
out vec4 color;

void main () {

    // À changer:
	color = fragmentColor;
}