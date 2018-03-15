#version 330 core
out vec3 color;

in vec3 vertex_normal;
in vec2 vertex_tex;
in vec3 vertex_pos;

in vec4 gl_FragCoord;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
	color = texture(tex, vertex_tex).rgb;

	vec3 n = normalize(vertex_normal);
	vec3 lp=vec3(0,0,-100);
	vec3 ld = normalize(vertex_pos - lp);
	float diffuse = dot(n,ld);

	color += diffuse*.05;

	vec3 campos = vec3(0, 0, .3);

	vec3 cd = normalize(campos - vertex_pos);
	vec3 h = normalize(cd+ld);
	float spec = dot(n,h);
	spec = clamp(spec,0,1);
	spec = pow(spec,20);
	color += vec3(1,1,1)*spec/6;

}
