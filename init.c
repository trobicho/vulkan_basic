/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 18:50:38 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/20 02:33:58 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "validation_layer.h"
#include "queue.h"
#include "device.h"
#include "init_swap_chain.h"
#include "gpu_pipeline.h"
#include "command_buffer.h"
#include "descriptor.h"
#include "image.h"

static int	vulk_create_instance(t_vulk *vulk)
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	uint32_t	val_layer_count;
	char		*val_layer[1] = {"VK_LAYER_KHRONOS_validation"};
	uint32_t	ext_count;
	char		**ext;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	val_layer_count = 1;
	ext = get_extensions(vulk->debug, &ext_count, val_layer_count, val_layer);
	if (ext == NULL && ext_count > 0)
	{
		printf("failed to get extensions!\n");
		return (-1);
	}
	createInfo.enabledExtensionCount = ext_count;
	createInfo.ppEnabledExtensionNames = ext;
	createInfo.enabledLayerCount = 0;
	if (vulk->debug == 1)
	{
		if (!val_layer_check(val_layer, val_layer_count))
		{
			printf("no corresponding validation layer found\n");
			return (-1);
		}
		createInfo.enabledLayerCount = val_layer_count;
		createInfo.ppEnabledLayerNames = val_layer;
	}

	if (vkCreateInstance(&createInfo, NULL, &vulk->instance) != VK_SUCCESS)
	{
		printf("failed to create instance!\n");
		return (-1);
	}
	return (0);
}

uint32_t	find_memory_type(t_vulk *vulk, uint32_t typeFilter,
	VkMemoryPropertyFlags properties)
{
	uint32_t							i;
	VkPhysicalDeviceMemoryProperties	memProperties;

	vkGetPhysicalDeviceMemoryProperties(vulk->dev_phy, &memProperties);
	for (i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && 
			(memProperties.memoryTypes[i].propertyFlags & properties)
			== properties) 
			return (i);
	}
	return (0);
}

static int	render_pass_create(t_vulk *vulk)
{
	static VkAttachmentDescription color_attachment;
	static VkAttachmentReference	color_attachment_ref;
	static VkSubpassDescription	subpass;
	static VkSubpassDependency	subpass_dep;
	VkRenderPassCreateInfo	render_pass_info;

	subpass = (VkSubpassDescription){};
	subpass_dep = (VkSubpassDependency){};
	color_attachment_ref = (VkAttachmentReference){};
	color_attachment = (VkAttachmentDescription){};
	color_attachment.format = vulk->swap_chain_img_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dep.dstSubpass = 0;
	subpass_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpass_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &subpass_dep;
	render_pass_info = (VkRenderPassCreateInfo){};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	if (vkCreateRenderPass(vulk->device, &render_pass_info, NULL
			, &vulk->render_pass) != VK_SUCCESS)
	{
		printf("failed to create render pass!\n");
		return (-1);
	}
	return (0);
}


static int	vulk_create_surface(t_vulk *vulk)
{
	if (glfwCreateWindowSurface(vulk->instance, vulk->win, NULL, &vulk->surface)
		!= VK_SUCCESS)
	{
		printf("failed to create window surface!\n");
		return (-1);
	}
	return (0);
}

static int	vulk_swap_chain(t_vulk *vulk)
{
	t_swap_chain_detail			detail;
	VkSurfaceFormatKHR			format;
	VkPresentModeKHR			present_mode;
	VkExtent2D					extent;
	uint32_t					img_count;
	VkSwapchainCreateInfoKHR	swap_info = {};

	detail = swap_get_detail(vulk->dev_phy, vulk->surface);
	format = swap_choose_format(detail.format);
	present_mode = swap_choose_present_mode(detail.present_mode);
	extent = swap_choose_extent(detail.capability);
	img_count = detail.capability.minImageCount + 1;
	if (detail.capability.maxImageCount > 0
		&& img_count > detail.capability.maxImageCount)
		img_count = detail.capability.maxImageCount;
	swap_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swap_info.surface = vulk->surface;
	swap_info.minImageCount = img_count;
	swap_info.imageFormat = format.format;
	swap_info.imageColorSpace = format.colorSpace;
	swap_info.imageExtent = extent;
	swap_info.imageArrayLayers = 1;
	swap_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swap_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swap_info.queueFamilyIndexCount = 0;
	swap_info.pQueueFamilyIndices = NULL;
	swap_info.preTransform = detail.capability.currentTransform;
	swap_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swap_info.presentMode = present_mode;
	swap_info.clipped = VK_TRUE;
	swap_info.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(vulk->device, &swap_info, NULL, &vulk->swap_chain)
		!= VK_SUCCESS)
	{
		printf("failed to create swap chain!\n");
		return (-1);
	}
	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	if (img_count <= 0)
		return (-1);
	if ((vulk->swap_chain_img = (VkImage*)malloc(sizeof(VkImage) * img_count))
		== NULL)
		return (-1);
	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain
		, &img_count, vulk->swap_chain_img);
	vulk->swap_chain_img_format = format.format;
	vulk->swap_chain_extent = extent;
	return (0);
}

static int	vulk_image_view(t_vulk *vulk)
{
	int						i;
	uint32_t				img_count;
	VkImageViewCreateInfo	view_info;

	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	vulk->image_view = (VkImageView*)malloc(sizeof(VkImageView) * img_count);
	if (vulk->image_view == NULL)
		return (-1);
	i = 0;
	view_info = (VkImageViewCreateInfo){};
	while (i < img_count)
	{
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = vulk->swap_chain_img[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = vulk->swap_chain_img_format;
		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		if (vkCreateImageView(vulk->device, &view_info, NULL
				, &vulk->image_view[i]) != VK_SUCCESS)
		{
			printf("failed to create image views!");
			return (-1);
		}
		i++;
	}
	return (0);
}

static int	framebuffer_create(t_vulk *vulk)
{
	int						i;
	uint32_t				img_count;
	VkFramebufferCreateInfo	framebuffer_info;

	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	if ((vulk->framebuffer = (VkFramebuffer*)
				malloc(sizeof(VkFramebuffer) * img_count)) == NULL)
		return (-1);
	i = 0;
	framebuffer_info = (VkFramebufferCreateInfo){};
	while (i < img_count)
	{
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = vulk->render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = &vulk->image_view[i];
		framebuffer_info.width = vulk->swap_chain_extent.width;
		framebuffer_info.height = vulk->swap_chain_extent.height;
		framebuffer_info.layers = 1;
		if (vkCreateFramebuffer(vulk->device, &framebuffer_info, NULL
				, &vulk->framebuffer[i]) != VK_SUCCESS)
		{
			printf("failed to create framebuffer!\n");
			return (-1);
		}
		i++;
	}
	return (0);
}

static int	semaphore_create(t_vulk *vulk)
{
	VkSemaphoreCreateInfo	semaphore_info;

	semaphore_info = (VkSemaphoreCreateInfo){};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(vulk->device, &semaphore_info, NULL
			, &vulk->semaphore_image_available) != VK_SUCCESS
			|| vkCreateSemaphore(vulk->device, &semaphore_info, NULL
			, &vulk->semaphore_render_finish) != VK_SUCCESS)
	{
		printf("failed to create semaphore!\n");
		return (-1);
	}
	return (0);
}

int			vulk_init(t_vulk *vulk)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//vulk->win = glfwCreateWindow(2560, 1440, "Vulkan", NULL, NULL);
	vulk->win = glfwCreateWindow(800, 800, "Vulkan", NULL, NULL);
	if (vulk_create_instance(vulk) == -1)
		return (-1);
	if (vulk_create_surface(vulk) == -1)
		return (-1);
	if (vulk_physical_device(vulk) == -1)
		return (-1);
	if (vulk_logical_device(vulk) == -1)
		return (-1);
	if (vulk_swap_chain(vulk) == -1)
		return (-1);
	if (create_image(vulk, VK_FORMAT_R8G8B8A8_UNORM) == -1)
		return (-1);
	if (vulk_image_view(vulk) == -1)
		return (-1);
	if (render_pass_create(vulk) == -1)
		return (-1);
	if (create_desc_layout(vulk) == -1)
		return (-1);
	if (gpu_pipeline_create(vulk) == -1)
		return (-1);
	if (framebuffer_create(vulk) == -1)
		return (-1);
	if (command_pool_create(vulk) == -1)
		return (-1);
	if (create_desc_pool(vulk) == -1)
		return (-1);
	if (create_desc_set(vulk) == -1)
		return (-1);
	if (compute_command_buffer_create(vulk) == -1)
		return (-1);
	if (command_buffer_create(vulk) == -1)
		return (-1);
	if (semaphore_create(vulk) == -1)
		return (-1);
	return (0);
}

void		vulk_close(t_vulk *vulk)
{
	int			i;
	uint32_t	img_count;

	vkDestroySemaphore(vulk->device, vulk->semaphore_render_finish, NULL);
	vkDestroyCommandPool(vulk->device, vulk->command_pool, NULL);
	vkDestroyPipeline(vulk->device, vulk->graphics_pipeline, NULL);
	vkDestroyPipelineLayout(vulk->device, vulk->pipeline_layout, NULL);
	vkDestroyRenderPass(vulk->device, vulk->render_pass, NULL);
	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	i = 0;
	while (i < img_count)
	{
		vkDestroyImageView(vulk->device, vulk->image_view[i], NULL);
		vkDestroyFramebuffer(vulk->device, vulk->framebuffer[i], NULL);
		i++;
	}
	vkDestroySwapchainKHR(vulk->device, vulk->swap_chain, NULL);
	vkDestroySurfaceKHR(vulk->instance, vulk->surface, NULL);
	vkDestroyInstance(vulk->instance, NULL);
	vkDestroyDevice(vulk->device, NULL);
	glfwDestroyWindow(vulk->win);
	glfwTerminate();
}
