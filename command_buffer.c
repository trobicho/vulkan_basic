/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_buffer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 08:49:02 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/20 02:40:38 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "descriptor.h"

int	compute_command_buffer_create(t_vulk *vulk)
{
	int							i;
	uint32_t					img_count;
	VkCommandBufferAllocateInfo alloc_info;
	VkCommandBufferBeginInfo	begin_info;
	VkImageMemoryBarrier		img_mem_barrier;

	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	if ((vulk->compute.command_buffer = (VkCommandBuffer*)
				malloc(sizeof(VkCommandBuffer) * img_count)) == NULL)
		return (-1);
	alloc_info = (VkCommandBufferAllocateInfo){};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vulk->command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = img_count;
	if (vkAllocateCommandBuffers(vulk->device, &alloc_info
			, vulk->compute.command_buffer) != VK_SUCCESS)
	{
		printf("failed to allocate compute command buffer!\n");
		return (-1);
	}
	i = 0;
	begin_info = (VkCommandBufferBeginInfo){};
	while (i < img_count)
	{
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		begin_info.pInheritanceInfo = NULL;
		update_compute_img_desc_set(vulk->device, vulk->compute.desc_set_pre[i]
			, vulk->image_view[i], VK_IMAGE_LAYOUT_GENERAL);
		if (vkBeginCommandBuffer(vulk->compute.command_buffer[i], &begin_info)
				!= VK_SUCCESS)
		{
			printf("failed to begin recording compute command buffer!\n");
			return (-1);
		}
		img_mem_barrier = (VkImageMemoryBarrier){};
		img_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		img_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		img_mem_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_mem_barrier.image = vulk->swap_chain_img[i];
		img_mem_barrier.subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		img_mem_barrier.srcAccessMask = 0;
		img_mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT
			| VK_ACCESS_SHADER_WRITE_BIT;
		vkCmdPipelineBarrier(vulk->compute.command_buffer[i]
			, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, VK_DEPENDENCY_BY_REGION_BIT
			, 0, NULL, 0, NULL, 1, &img_mem_barrier);
		vkCmdBindPipeline(vulk->compute.command_buffer[i]
				, VK_PIPELINE_BIND_POINT_COMPUTE, vulk->compute.pipeline);
		vkCmdBindDescriptorSets(vulk->compute.command_buffer[i]
				,VK_PIPELINE_BIND_POINT_COMPUTE, vulk->compute.pipeline_layout
				, 0, 1, &vulk->compute.desc_set_pre[i], 0, NULL);
		vkCmdDispatch(vulk->compute.command_buffer[i]
				, vulk->swap_chain_extent.width / 16
				, vulk->swap_chain_extent.height / 16, 1);
		img_mem_barrier = (VkImageMemoryBarrier){};
		img_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		img_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_mem_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		img_mem_barrier.image = vulk->swap_chain_img[i];
		img_mem_barrier.subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		img_mem_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		img_mem_barrier.dstAccessMask = 0; 
		vkCmdPipelineBarrier(vulk->compute.command_buffer[i]
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
			, VK_DEPENDENCY_BY_REGION_BIT
			, 0, NULL, 0, NULL, 1, &img_mem_barrier);
		if (vkEndCommandBuffer(vulk->compute.command_buffer[i]) != VK_SUCCESS)
		{
			printf("failed to record compute command buffer!\n");
			return (-1);
		}
		i++;
	}
	return (0);
}

int	command_buffer_create(t_vulk *vulk)
{
	int							i;
	uint32_t					img_count;
	VkCommandBufferAllocateInfo alloc_info;
	VkCommandBufferBeginInfo	begin_info;
	VkRenderPassBeginInfo		render_pass_info;
	VkClearValue				clear_color;
	VkImageMemoryBarrier		img_mem_barrier;

	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	if ((vulk->command_buffer = (VkCommandBuffer*)
				malloc(sizeof(VkCommandBuffer) * img_count)) == NULL)
		return (-1);
	alloc_info = (VkCommandBufferAllocateInfo){};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = vulk->command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = img_count;
	if (vkAllocateCommandBuffers(vulk->device, &alloc_info
			, vulk->command_buffer) != VK_SUCCESS)
	{
		printf("failed to allocate command buffer!\n");
		return (-1);
	}
	i = 0;
	begin_info = (VkCommandBufferBeginInfo){};
	clear_color = (VkClearValue){0.0f, 0.0f, 0.0f, 1.0f};
	while (i < img_count)
	{
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		begin_info.pInheritanceInfo = NULL;
		update_compute_post_img_desc_set(vulk->device
			, vulk->compute.desc_set_post[i], vulk->image_view[i]
			, VK_IMAGE_LAYOUT_GENERAL);
		if (vkBeginCommandBuffer(vulk->command_buffer[i], &begin_info)
			!= VK_SUCCESS)
		{
			printf("failed to begin recording command buffer!\n");
			return (-1);
		}
		render_pass_info = (VkRenderPassBeginInfo){};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = vulk->render_pass;
		render_pass_info.framebuffer = vulk->framebuffer[i];
		render_pass_info.renderArea.offset = (VkOffset2D){0, 0};
		render_pass_info.renderArea.extent = vulk->swap_chain_extent;
		render_pass_info.clearValueCount = 0;
		render_pass_info.pClearValues = &clear_color;
		img_mem_barrier = (VkImageMemoryBarrier){};
		img_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		img_mem_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_mem_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		img_mem_barrier.image = vulk->swap_chain_img[i];
		img_mem_barrier.subresourceRange = (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		img_mem_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		img_mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdBeginRenderPass(vulk->command_buffer[i], &render_pass_info
			, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdPipelineBarrier(vulk->command_buffer[i]
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_DEPENDENCY_BY_REGION_BIT
			, 0, NULL, 0, NULL, 1, &img_mem_barrier);
		vkCmdBindDescriptorSets(vulk->command_buffer[i]
			,VK_PIPELINE_BIND_POINT_COMPUTE, vulk->pipeline_layout
			, 0, 1, &vulk->compute.desc_set_post[i], 0, NULL);
		vkCmdBindPipeline(vulk->command_buffer[i]
			, VK_PIPELINE_BIND_POINT_GRAPHICS, vulk->graphics_pipeline);
		vkCmdDraw(vulk->command_buffer[i], 4, 1, 0, 0);
		vkCmdEndRenderPass(vulk->command_buffer[i]);
		if (vkEndCommandBuffer(vulk->command_buffer[i]) != VK_SUCCESS)
		{
			printf("failed to record command buffer!\n");
			return (-1);
		}
		i++;
	}
	return (0);
}

int	command_pool_create(t_vulk *vulk)
{
	int						queue_indice;
	VkCommandPoolCreateInfo	pool_info;

	queue_indice = get_graphic_queue_family(vulk->dev_phy, vulk->surface);
	pool_info = (VkCommandPoolCreateInfo){};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queue_indice;
	pool_info.flags = 0;
	if (vkCreateCommandPool(vulk->device, &pool_info, NULL
			, &vulk->command_pool) != VK_SUCCESS)
	{
		printf("failed to create command pool!\n");
		return (-1);
	}
	return (0);
}
