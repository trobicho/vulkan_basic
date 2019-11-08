/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 17:55:04 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/08 18:12:45 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

/*
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/vec4x4.hpp>
*/

#include "init.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

static int draw_frame(t_vulk *vulk)
{
	uint32_t				img_index;
	VkSubmitInfo			submit_info;
	VkPipelineStageFlags	wait_stage;
	VkPresentInfoKHR		present_info;

	vkAcquireNextImageKHR(vulk->device, vulk->swap_chain, UINT64_MAX,
		vulk->semaphore_image_available, VK_NULL_HANDLE, &img_index);
	submit_info = (VkSubmitInfo){};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &vulk->semaphore_image_available;
	wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vulk->command_buffer[img_index];
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &vulk->semaphore_render_finish;
	if (vkQueueSubmit(vulk->queue_graphic, 1, &submit_info, VK_NULL_HANDLE)
		!= VK_SUCCESS)
	{
		printf("failed to submit draw command buffer!\n");
		return (-1);
	}
	present_info = (VkPresentInfoKHR){};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &vulk->semaphore_render_finish;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vulk->swap_chain;
	present_info.pImageIndices = &img_index;
	present_info.pResults = NULL;
	vkQueuePresentKHR(vulk->queue_graphic, &present_info);
	vkQueueWaitIdle(vulk->queue_graphic);
	return (0);
}

static void	main_loop(t_vulk *vulk)
{
	while(!glfwWindowShouldClose(vulk->win))
	{
		glfwPollEvents();
		draw_frame(vulk);
	}
	vkDeviceWaitIdle(vulk->device);
}

int			main(int ac, char **av)
{
	t_vulk	vulk;

	vulk.debug = 1;
	if (vulk_init(&vulk) == -1)
		return (EXIT_FAILURE);

	main_loop(&vulk);
	vulk_close(&vulk);
	return (EXIT_SUCCESS);
}
