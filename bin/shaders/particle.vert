/*#version 330

// vertex attributes
in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec2 tc;	// texture coordinate

// uniforms
uniform vec2 center;
uniform float scale;
uniform float aspect_ratio;

void main()
{
	vec3 pos = position * scale;
	pos.xy *= aspect_ratio>1 ? vec2(1/aspect_ratio,1) : vec2(1,aspect_ratio); // tricky aspect correction

	gl_Position = vec4(center, 0, 0) +  vec4(pos, 1);
	tc = texcoord;
}
*/
#version 330

// input attributes of vertices
in vec3 position;	
in vec3 normal;
in vec2 texcoord;

// outputs of vertex shader = input to fragment shader
// out vec4 gl_Position: a built-in output variable that should be written in main()
out vec3 norm;	// the second output: not used yet
out vec2 tc;	// the third output: not used yet

// uniform variables
uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	aspect_matrix;	// tricky 4x4 aspect-correction matrix
uniform mat4	projection_matrix;
uniform mat4	view_matrix;

void main()
{
	/*gl_Position = aspect_matrix*model_matrix*vec4(position,1);

	// another output passed via varying variable
	norm = normal;
	tc = texcoord;*/

	vec4 wpos = model_matrix * vec4(position,1);
	vec4 epos = view_matrix * wpos;
	gl_Position = projection_matrix * epos;

	// pass eye-coordinate normal to fragment shader
	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	tc = texcoord;
}