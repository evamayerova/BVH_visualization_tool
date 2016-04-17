#version 430

struct PointLight {
    vec3 position;
    vec3 diffuse;
};

in vec3 pos_out;
in vec3 nor_out;
in vec3 color_out;
PointLight light;
out vec4 resultColor;

void main()
{
	light.position = vec3(0.3, 1.126414, 1.0);
	light.diffuse = vec3(1.0);

    vec3 N = normalize(nor_out);
    vec3 L = normalize(light.position - pos_out);
	float NdotL = max(0.0, dot(N, L));

    //vec3 C = normalize(camPos - pos_out);
    //vec3 S = normalize(C+L);
    //float SdotN = max(0.0, dot(S, N));
    //vec3 ambient_reflected = light.ambient * material.ambient;
    //vec3 specular_reflected = pow(SdotN, material.shininess) * light.specular * material.specular;

	vec3 diffuse_reflected = NdotL * light.diffuse * color_out;
	resultColor = vec4(diffuse_reflected, 1.0);

    //resultColor = color * vec4(diffuse_reflected + ambient_reflected + specular_reflected, 1.0);
  
}

