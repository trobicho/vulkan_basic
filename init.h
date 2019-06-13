/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 18:50:35 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/13 12:59:18 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_HEADER
# define INIT_HEADER

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"

typedef struct	s_compute
{
	VkPipelineLayout		pipeline_layout;
	VkPipeline				pipeline;
	VkDescriptorImageInfo	img_desc;
	VkDescriptorSetLayout	desc_set_layout_pre;
	VkDescriptorSetLayout	desc_set_layout_post;
	VkDescriptorSet			desc_set_pre;
	VkDescriptorSet			desc_set_post;
	VkSampler				sampler;
	VkCommandBuffer			command_buffer;
}				t_compute;

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
	VkDescriptorPool	desc_pool;
	VkSemaphore			semaphore_image_available;
	VkSemaphore			semaphore_render_finish;
	t_compute			compute;
}				t_vulk;

int				vulk_init(t_vulk *vulk);
void			vulk_close(t_vulk *vulk);
#endif
