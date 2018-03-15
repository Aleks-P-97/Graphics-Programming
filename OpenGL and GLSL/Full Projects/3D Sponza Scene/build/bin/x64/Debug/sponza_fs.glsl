#version 330

uniform sampler2D tex;

out vec4 fragment_colour;

in vec4 varying_colour;
in vec2 varying_texCoord;

void main(void)
{
	fragment_colour = 0.5*0.5+varying_colour*texture(tex, varying_texCoord);
}
