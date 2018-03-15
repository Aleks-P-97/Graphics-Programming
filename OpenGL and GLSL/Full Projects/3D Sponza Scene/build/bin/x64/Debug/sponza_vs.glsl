#version 330

in vec2 texCoord;
in vec3 vertex_position;
in vec3 vertex_normal;//Make sure its vec3

uniform mat4 projection_view_xform;
uniform mat4 model_xform;

out vec4 varying_colour;
out vec2 varying_texCoord;

void main(void)
{
	vec4 normal_colour = vec4(vertex_normal, 0); //TODO: get rid of negative colours without using MAD
	varying_colour = model_xform * normal_colour;
	varying_texCoord = texCoord;

    gl_Position =  projection_view_xform * model_xform * vec4(vertex_position, 1.0);
}
