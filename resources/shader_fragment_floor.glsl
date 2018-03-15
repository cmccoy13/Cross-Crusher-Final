#version 330 core
out vec3 color;
in vec3 vertex_color;

in vec4 gl_FragCoord;

void main()
{
	if(gl_FragCoord.x <= 635)
	{
		color = normalize(vec3(131, 217, 4)) + 0.2;
	}
	if(gl_FragCoord.x >= 864)
	{
		color = normalize(vec3(234, 96, 3)) + 0.2;
	}
	if(gl_FragCoord.y <= 382)
	{
		color = normalize(vec3(255, 19, 0)) + 0.2;
	}
	if(gl_FragCoord.y >= 617)
	{
		
		color = normalize(vec3(157, 200, 8)) + 0.2;
	}
}
