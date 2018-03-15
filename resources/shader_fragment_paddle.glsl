#version 330 core
out vec3 color;
in vec3 vertex_color;

in vec4 gl_FragCoord;

void main()
{
	color = vec3(255, 255, 255);

	float x = gl_FragCoord.x;
	float y = gl_FragCoord.y;
	float z = gl_FragCoord.z;
	
	color = normalize(vec3(x, y, z));
	/*
	if(gl_FragCoord.x <= 635 && gl_FragCoord.y > 382 && gl_FragCoord.y < 617)
	{
		color = normalize(vec3(76, 151, 4));
	}
	if(gl_FragCoord.x >= 864)
	{
		color = normalize(vec3(174, 39, 5));
	}
	if(gl_FragCoord.y <= 382)
	{
		color = normalize(vec3(187, 0, 6));
	}
	if(gl_FragCoord.y >= 617)
	{
		
		color = normalize(vec3(100, 135, 3));
	}
	*/

	if(gl_FragCoord.z > 0.9662)
	{
		color -= z/3;
	}
}
