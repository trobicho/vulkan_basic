#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform sampler2D sampler_color;
layout (location = 0) out vec4 out_color;

void	main()
{
	out_color = texture(sampler_color, gl_FragCoord.xy);
}
