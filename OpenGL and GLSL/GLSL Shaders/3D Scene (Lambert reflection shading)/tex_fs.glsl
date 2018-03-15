#version 330

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float field_of_view;
    mat4 projection_view_xform;
};

uniform float time_seconds;
uniform vec3 camera_position;
uniform Light light_source;
uniform sampler2D sampler_tex0;
uniform sampler2D sampler_tex1;  //Does not exist in the directory
uniform sampler2D sampler_tex2;

out vec4 fragment_colour;

in vec3 varying_normal;
in vec2 varying_texcoord; 
in vec3 varying_position;

void main(void)
{
	vec3 P = varying_position;
    vec3 N = normalize(varying_normal);
	vec4 tex_colouring = vec4(texture(sampler_tex0, varying_texcoord).rgb, 0);
	
	//Attenuation constants
	const float ATTENUATION_CONSTANT = 1;
    const float ATTENUATION_LINEAR = 0.0;
    const float ATTENUATION_QUADRATIC = 0.0001;
	
	float ambientIntensity = 0.2;
	vec3 ambientCoefficient = vec3(0.85, 0.80, 0.66);//Material colour - Ka
	vec3 ambientLight = ambientIntensity * ambientCoefficient; //Ia*Ka
	
	vec3 diffusedCoefficient = vec3(0.85, 0.80, 0.66);//Material colour - Kd
	
	//Calc intensity
	vec3 I; //needs to be a vec3 so that the colour will be maintained and no data is lost.
	
	vec3 L = normalize(light_source.position - P);
	float d = distance(light_source.position, P);
	float fd = 1.0 / (ATTENUATION_CONSTANT
                    + d * ATTENUATION_LINEAR
                    + d * d * ATTENUATION_QUADRATIC);
	
	I += ambientLight;
	I += diffusedCoefficient * fd * max(dot(L, N), 0.0); 

    fragment_colour = vec4(I, 1.0) * tex_colouring;
}
