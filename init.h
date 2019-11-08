/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 18:50:35 by trobicho          #+#    #+#             */
/*   Updated: 2019/11/08 18:12:32 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_HEADER
# define INIT_HEADER

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"

typedef struct	s_vulk
{
	GLFWwindow			*win;
	VkInstance			instance;
	VkDevice			device;
	VkPhysicalDevice	dev_phy;
	VkQueue				queue_graphic;
	VkSurfaceKHR		surface;
	VkSwapchainKHR		swap_chain;
	VkImage				*swap_chain_img;
	VkImageView			*image_view;
	VkFormat			swap_chain_img_format;
	VkExtent2D			swap_chain_extent;
	VkRenderPass		render_pass;
	VkPipelineLayout	pipeline_layout;
	VkPipeline			graphics_pipeline;
	VkFramebuffer		*framebuffer;
	VkCommandPool		command_pool;
	VkCommandBuffer		*command_buffer;
	VkSemaphore			semaphore_image_available;
	VkSemaphore			semaphore_render_finish;
	int					debug;
}				t_vulk;

int				vulk_init(t_vulk *vulk);
void			vulk_close(t_vulk *vulk);
#endif
