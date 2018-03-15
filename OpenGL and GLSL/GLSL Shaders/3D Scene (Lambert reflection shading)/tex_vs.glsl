#version 330

uniform float time_seconds;
uniform mat4 projection_view_model_xform;
uniform mat4 projection_xform;
uniform mat4 view_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texcoord;

out vec3 varying_normal;
out vec2 varying_texcoord; 
out vec3 varying_position;

void main(void)
{
	varying_normal = mat3(model_xform)*vertex_normal;
	varying_texcoord = vertex_texcoord;
	varying_position = mat4x3(model_xform) * vec4(vertex_position, 1.0);

    gl_Position = projection_view_model_xform * vec4(vertex_position, 1.0);
}
