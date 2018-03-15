/*
 * Tutorial: This is the vertex shader.  The value written to the gl_Position
 *           variable will be used as the vertex position by the rasterizer.
 *           The gl_Position value must be XYZW.
 */

#version 330

uniform float time_seconds;
uniform mat4 mystery_xform;

in vec3 vertex_position;

out vec3 varying_something;


void main(void)
{
	float offset = sin(time_seconds);
	
	varying_something = vertex_position;
	
	/* APPLYING TRANSFORMATIONS
	 *
	 * Multiplying by the offset give the scaling animation.
	 * Can also do vec4(vertex_position*offset, 1.0) to get the same animation.
	 * The mystery_xform is rotation.
	 */
	 
    gl_Position = mystery_xform*vec4(vertex_position.x*offset, vertex_position.y*offset, vertex_position.z*offset, 1.0); //gl_Position is vec4
}


