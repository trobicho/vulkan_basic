/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu_pipeline.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 17:58:08 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/13 12:52:07 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "init.h"
#include "shader.h"
#include "pipeline_utils.h"
#include "stdlib.h"

#include "stdio.h"

static VkPipelineShaderStageCreateInfo	*pipe_shader_create(
	VkShaderModule vert_mod, VkShaderModule frag_mod)
{
	VkPipelineShaderStageCreateInfo			vert_info;
	VkPipelineShaderStageCreateInfo			frag_info;
	VkPipelineShaderStageCreateInfo			*ret_info;

	vert_info = (VkPipelineShaderStageCreateInfo){};
	frag_info = (VkPipelineShaderStageCreateInfo){};

	vert_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_info.module = vert_mod;
	vert_info.pName = "main";

	frag_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_info.module = frag_mod;
	frag_info.pName = "main";
	if ((ret_info = malloc(sizeof(*ret_info) * 2)) == NULL)
		return (NULL);
	ret_info[0] = vert_info;
	ret_info[1] = frag_info;

	return (ret_info);
}

static int		gpu_compute_pipe_create(t_vulk *vulk)
{
	char							*comp_shader;
	int								comp_size;
	VkShaderModule					comp_shader_module;
	VkPipelineLayoutCreateInfo		pipeline_layout_info;
	VkComputePipelineCreateInfo		pipeline_info;
	VkPipelineShaderStageCreateInfo	comp_info;

	comp_shader = shader_load("shader/comp.spv", &comp_size);
	printf("compute shader size (%d)\n", comp_size);
	if (shader_create_module(vulk, comp_shader, comp_size
		, &comp_shader_module) == -1)
		return (-1);


	pipeline_layout_info = (VkPipelineLayoutCreateInfo){};
	pipeline_layout_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(vulk->device, &pipeline_layout_info, NULL
		, &vulk->compute.pipeline_layout) != VK_SUCCESS)
	{
		printf("failed to create compute pipeline layout!\n");
		return (-1);
	}
	pipeline_info = (VkComputePipelineCreateInfo){};
	pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipeline_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	pipeline_info.stage.module = comp_shader_module;
	pipeline_info.stage.pName = "main";
	pipeline_info.layout = vulk->compute.pipeline_layout;
	if (vkCreateComputePipelines(vulk->device, VK_NULL_HANDLE, 1
			, &pipeline_info, NULL, &vulk->compute.pipeline) != VK_SUCCESS)
	{
		printf("failed to create compute pipeline!");
		return (-1);
	}
	vkDestroyShaderModule(vulk->device, comp_shader_module, NULL);
	return (0);
}

int	gpu_pipeline_create(t_vulk *vulk)
{
	char							*vert_shader;
	char							*frag_shader;
	int								vert_size;
	int								frag_size;
	VkShaderModule					vert_shader_module;
	VkShaderModule					frag_shader_module;
	VkPipelineLayoutCreateInfo		pipeline_layout_info;
	VkGraphicsPipelineCreateInfo	pipeline_info;

	VkPipelineRasterizationStateCreateInfo	rasterizer_info;
	VkPipelineViewportStateCreateInfo		viewport_info;
	VkPipelineInputAssemblyStateCreateInfo	input_assembly;
	VkPipelineColorBlendStateCreateInfo		color_blend_info;
	VkPipelineMultisampleStateCreateInfo	multisampling_info;
	VkPipelineVertexInputStateCreateInfo	vert_input_info;
	VkPipelineShaderStageCreateInfo			*shader_stage;

	gpu_compute_pipe_create(vulk);
	vert_shader = shader_load("shader/vert.spv", &vert_size);
	frag_shader = shader_load("shader/frag.spv", &frag_size);
	printf("shader size (%d, %d)\n", vert_size, frag_size);
	if (shader_create_module(vulk, vert_shader, vert_size
		, &vert_shader_module) == -1)
		return (-1);
	if (shader_create_module(vulk, frag_shader, frag_size
		, &frag_shader_module) == -1)
		return (-1);

	shader_stage = pipe_shader_create(vert_shader_module, frag_shader_module);
	if (shader_stage == NULL)
		return (-1);
	vert_input_info = vert_input_create();
	input_assembly = input_assembly_create();
	viewport_info = viewport_create(vulk->swap_chain_extent);
	rasterizer_info = rasterizer_create();
	multisampling_info = multisampling_create();
	color_blend_info = color_blend_create();

	pipeline_layout_info = (VkPipelineLayoutCreateInfo){};
	pipeline_layout_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(vulk->device, &pipeline_layout_info, NULL
		, &vulk->pipeline_layout) != VK_SUCCESS)
	{
		printf("failed to create pipeline layout!\n");
		return (-1);
	}
	pipeline_info = (VkGraphicsPipelineCreateInfo){};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stage;
	pipeline_info.pVertexInputState = &vert_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = NULL;
	pipeline_info.pColorBlendState = &color_blend_info;
	pipeline_info.pDynamicState = NULL;
	pipeline_info.layout = vulk->pipeline_layout;
	pipeline_info.renderPass = vulk->render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = -1;
	if (vkCreateGraphicsPipelines(vulk->device, VK_NULL_HANDLE, 1
			, &pipeline_info, NULL, &vulk->graphics_pipeline) != VK_SUCCESS)
	{
		printf("failed to create graphics pipeline!");
		return (-1);
	}

	vkDestroyShaderModule(vulk->device, frag_shader_module, NULL);
	vkDestroyShaderModule(vulk->device, vert_shader_module, NULL);
	return (0);
}
