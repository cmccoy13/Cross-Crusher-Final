#version 330 core
out vec3 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec4 gl_FragCoord;
uniform vec4 Center;
void main()
{
	color = vec3(0, 0, 0);

	vec3 n = normalize(vertex_normal);
	vec3 lp=vec3(0,0, 1000);
	vec3 ld = normalize(lp - vertex_pos);
	float diffuse = dot(n,ld);

	color += diffuse*.8;

	vec3 campos = vec3(0, 0, .3);

	vec3 cd = normalize(campos - vertex_pos);
	vec3 h = normalize(cd+ld);
	float spec = dot(n,h);
	spec = clamp(spec,0,1);
	spec = pow(spec,10);
	color += spec/2;
}
