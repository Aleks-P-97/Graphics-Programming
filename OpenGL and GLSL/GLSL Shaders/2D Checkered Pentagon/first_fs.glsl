/*
 * Tutorial: This is the fragment shader.  The value written to the colour
 *           variable will be written to the pixel in the framebuffer. The
 *           colour value must be RGBA.
 */

#version 330

uniform float time_seconds;

in vec3 varying_something;

out vec4 fragment_colour;

void main(void)
{
	float checkSize = 10;
  float fmodResult = mod(floor(checkSize * varying_something.x) + floor(checkSize * varying_something.y), 2.0);

  if (fmodResult < 1.0) 
  {
    fragment_colour =  vec4(1,1,1,1);
  } 
  else 
  {
    fragment_colour = vec4(0,0,0,1);
  }	
}
