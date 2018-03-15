#version 330 core
out vec3 color;
in vec3 vertex_color;

in vec4 gl_FragCoord;

void main()
{	
	color = normalize(vec3(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z));

	if(gl_FragCoord.z > 0.9662)
	{
		color -= gl_FragCoord.z/4;
	}
}
