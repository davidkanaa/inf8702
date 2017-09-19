#version 430 core
layout(location = 0) in vec3 vp;
uniform sampler2D colorMap;
out vec3 fragTexCoord;

void main (void)
{
   gl_Position = vec4(vp,1.0);
   fragTexCoord = vp;
}
