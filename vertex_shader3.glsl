#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 incol;


uniform mat4 MVP;

out vec3 outcol;
out vec3 pos;

void main()
{
    
    pos = aPos;
    outcol=incol;
    gl_Position = MVP*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}