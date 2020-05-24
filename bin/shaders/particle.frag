/*#version 330

// input from vertex shader
in vec2 tc;

// the only output variable
out vec4 fragColor;

// texture sampler
uniform sampler2D TEX;
uniform vec4 color;

void main()
{
	fragColor = texture2D( TEX, tc ) * color;
}
*/

#version 330

// inputs from vertex shader
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform bool b_solid_color;
uniform vec4 solid_color;
uniform sampler2D TEX;
uniform vec4 color;

void main()
{
	//fragColor = b_solid_color ? solid_color : vec4(tc.xy,0,1);
	//fragColor = texture2D( TEX, tc );
	fragColor = b_solid_color ? solid_color : texture2D(TEX, tc);

	if(b_solid_color) fragColor = solid_color;
	else{
		vec4 texColor = texture2D(TEX, tc);
		if(texColor.a < 0.1) discard;
		fragColor = texColor;
	}
}