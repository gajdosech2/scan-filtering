#version 430
#include "visualizer/definitions.glsl"

/// INPUT
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_coords;
layout (location = 3) in vec3 in_color;
layout (location = 4) in vec4 in_tangents;

/// OUTPUT
layout (location = 0) out DefVertex out_vert;
layout (location = 4) out vec3 out_face_color;
layout (location = 5) out vec4 out_tangents;

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
	out_tangents.xyz = (transform * vec4(in_tangents.xyz, 0.0)).xyz;
	out_tangents.w = in_tangents.w;
}
