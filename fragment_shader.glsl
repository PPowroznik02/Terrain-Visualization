#version 330

in vec3 outcol;

out vec4 FragColor;

void main()
{
   gl_FragColor  = vec4(outcol.x, outcol.y,outcol.z, 1.0f);
} 
