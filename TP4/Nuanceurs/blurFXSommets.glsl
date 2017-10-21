#version 430 core
layout(location = 0) in vec3 vp;

uniform sampler2D colorMap;

out vec2 fragTexCoord;

void main (void)
{
   gl_Position = vec4(vp,1.0);
   
   // Passage des coordonnées normalisees de texture pour le nuanceur de fragment
   fragTexCoord = (vp.xy + vec2(1.0, 1.0)) / 2.0;
}
