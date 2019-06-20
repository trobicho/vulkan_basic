#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout (binding = 0) uniform sampler2D sampler_color;
layout (binding = 0, rgba8) uniform readonly image2D result_image;
layout (location = 0) out vec4 out_color;

void	main()
{
	//out_color = texture(sampler_color, gl_FragCoord.xy);
	out_color = imageLoad(result_image, ivec2(gl_FragCoord.xy));
	out_color.x += 0.3;
}
