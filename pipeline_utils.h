/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:21:14 by trobicho          #+#    #+#             */
/*   Updated: 2019/05/24 12:38:33 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPELINE_UTILS_H
# define PIPELINE_UTILS_H

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
# include "init.h"

VkPipelineMultisampleStateCreateInfo	multisampling_create(void);
VkPipelineRasterizationStateCreateInfo	rasterizer_create(void);
VkPipelineViewportStateCreateInfo		viewport_create(VkExtent2D extent);
VkPipelineColorBlendStateCreateInfo		color_blend_create(void);
VkPipelineInputAssemblyStateCreateInfo	input_assembly_create(void);
VkPipelineVertexInputStateCreateInfo	vert_input_create(void);
#endif
