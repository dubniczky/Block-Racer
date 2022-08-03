#version 140

in vec3 vs_out_pos;
in vec3 vs_out_norm;

out vec4 fs_out_col;

// Light
uniform vec3 light_dir = vec3(-1,-1,-1);
uniform vec3 La = vec3(0.2, 0.2, 0.2);
uniform vec3 Ld = vec3(0.4, 0.4, 0.4);
uniform vec3 Ls = vec3(1, 1, 1);

// Material
uniform vec3 Ka = vec3(0.0, 0.3, 0.6);
uniform vec3 Kd = vec3(0, 0.3, 0.6);
uniform vec3 Ks = vec3(0.9, 0.9, 0.9);

// Player
uniform vec3 eye_pos = vec3(0, 0, 10);

void main()
{	
	vec3 ambient = La*Ka;

	vec3 normal = normalize(vs_out_norm);
	vec3 to_light = normalize(-light_dir);
	
	float cosa = clamp(dot(normal, to_light), 0, 1);

	vec3 diffuse = cosa*Ld*Kd;
	vec3 e = normalize( eye_pos - vs_out_pos );
	vec3 r = reflect( -to_light, normal );
	float si = pow( clamp( dot(e, r), 0.0f, 1.0f ), 12);
	vec3 specular = Ls*Ks*si;

	fs_out_col = vec4(ambient + diffuse + specular, 1);
}