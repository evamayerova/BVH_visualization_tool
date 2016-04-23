#version 430

struct PointLight {
    vec3 position;
    vec3 diffuse;
};

in vec3 pos_out;
in vec3 nor_out;
in vec3 color_out;
uniform PointLight light;
out vec4 resultColor;

void main()
{
    vec3 N = normalize(nor_out);
    vec3 L = normalize(light.position - pos_out);
	float NdotL = max(0.0, dot(N, L));


	vec3 diffuse_reflected = NdotL * light.diffuse * color_out;
	resultColor = vec4(diffuse_reflected, 1.0);
  
}

