#version 430 core
uniform sampler2D colorMap;
in vec3 fragTexCoord;
out vec4 color;
void main (void) 
{
   color = texture(colorMap, (1+fragTexCoord.xy)/2.0);
}
