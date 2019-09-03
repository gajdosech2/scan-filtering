#version 430
#include "visualizer/definitions.glsl"

/// INPUT
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_coords;
layout (location = 3) in vec3 in_color;

/// OUTPUT
layout (location = 0) out DefVertex out_vert;

void main () 
{
	mat4 transform = MODELVIEW_MAT;
	if (use_instancing)
	{
		transform = transform * instance_matrices[gl_InstanceID] * preinstance_mat;
	}

	out_vert.position_eye = (transform * vec4(in_position, 1.0)).xyz;
	out_vert.normal_eye = (transform * vec4(in_normal, 0.0)).xyz;
	out_vert.coord = in_coords;
	out_vert.color = in_color;
}