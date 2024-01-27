#version 330

in vec3 pos;
in vec3 outcol;

out vec4 FragColor;

void main()
{

	//FragColor  = vec4(0.1, 0.1, pos.y, 1.0f);
	gl_FragColor  = vec4(outcol.x, outcol.y,outcol.z, 1.0f);

} 
