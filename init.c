/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 18:50:38 by trobicho          #+#    #+#             */
/*   Updated: 2019/05/26 14:56:07 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "init_swap_chain.h"
#include "gpu_pipeline.h"
#include "validation_layer.h"

static int	vulk_create_instance(t_vulk *vulk)
{
	char	**val_layer = {"VK_LAYER_KHRONOS_validation"};

	if (vulk->enable_val_layer && !val_layer_check(val_layer, 1))
	{
		printf("validation layers requested, but not available!\n");
		return (-1);
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	const char** glfw_ext;
	uint32_t glfw_ext_count;
	glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
	createInfo.enabledExtensionCount = glfw_ext_count;
	createInfo.ppEnabledExtensionNames = glfw_ext;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, NULL, &vulk->instance) != VK_SUCCESS)
	{
		printf("failed to create instance!\n");
		return (-1);
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
	subpass_dep.srcAccessMask = 0;
	subpass_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
		| VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
		printf("failed ton create render pass!\n");
		return (-1);
	}
	return (0);
}

static int	vulk_queue_family(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	int						i;
	uint32_t				queue_count;
	VkQueueFamilyProperties	*queue;
	VkBool32				present_support;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, NULL);
	if (queue_count <= 0 || (queue = (VkQueueFamilyProperties*)
			malloc(sizeof(*queue) * queue_count)) == NULL)
		return (-1);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, queue);
	i = 0;
	while (i < queue_count)
	{
		present_support = 0;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface
			, &present_support);
		if (queue[i].queueCount > 0 && queue[i].queueFlags
			& VK_QUEUE_GRAPHICS_BIT && present_support)
			return (i);
		i++;
	}
	return (-1);
}


static int	vulk_is_device_suitable(VkPhysicalDevice device,
	VkSurfaceKHR surface)
{
	VkPhysicalDeviceProperties	dev_prop;
	VkPhysicalDeviceFeatures	dev_feat;
	t_swap_chain_detail			detail;

	vkGetPhysicalDeviceProperties(device, &dev_prop);
	vkGetPhysicalDeviceFeatures(device, &dev_feat);
	if (vulk_queue_family(device, surface) == -1)
		return (0);
	detail = swap_get_detail(device, surface);
	if (detail.format == NULL || detail.present_mode == NULL)
		return (0);
	return (1);
}

#include <string.h>

static int	vulk_logical_device(t_vulk *vulk)
{
	int							dev_i;
	float						queue_priority;
	VkDeviceCreateInfo			dev_inf = {};
	VkDeviceQueueCreateInfo		queue_inf = {};
	VkPhysicalDeviceFeatures	dev_feat = {};
	char						*ext_name;

	ext_name = strdup(VK_KHR_SWAPCHAIN_EXTENSION_NAME); //libc!! free??
	if ((dev_i = vulk_queue_family(vulk->dev_phy, vulk->surface)) == -1)
		return (-1);
	queue_inf.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_inf.queueFamilyIndex = dev_i;
	queue_inf.queueCount = 1;
	queue_priority = 1.0f;
	queue_inf.pQueuePriorities = &queue_priority;
	dev_inf.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dev_inf.pQueueCreateInfos = &queue_inf;
	dev_inf.queueCreateInfoCount = 1;
	dev_inf.pEnabledFeatures = &dev_feat;
	dev_inf.enabledExtensionCount = 1;
	dev_inf.ppEnabledExtensionNames = &ext_name;
	dev_inf.enabledLayerCount = 0;
	if (vkCreateDevice(vulk->dev_phy, &dev_inf, NULL, &vulk->device) != VK_SUCCESS)
	{
		printf("failed to create logical device!\n");
		return (-1);
	}
	vkGetDeviceQueue(vulk->device, dev_i, 0, &vulk->queue_graphic);
	return (0);
}

static int	vulk_physical_device(t_vulk *vulk)
{
	int					i;
	uint32_t			device_count;
	VkPhysicalDevice	*device;

	vkEnumeratePhysicalDevices(vulk->instance, &device_count, NULL);
	if (device_count == 0)
	{
		printf("failed to find GPU with Vulkan support!\n");
		return (-1);
	}
	if ((device = (VkPhysicalDevice*)
		malloc(sizeof(*device) * device_count)) == NULL)
		return (-1);
	vkEnumeratePhysicalDevices(vulk->instance, &device_count, device);
	i = 0;
	vulk->dev_phy = VK_NULL_HANDLE;
	while (i < device_count && vulk->dev_phy == NULL)
	{
		if(vulk_is_device_suitable(device[i], vulk->surface))
			vulk->dev_phy = device[i];
		i++;
	}
	if (vulk->dev_phy == VK_NULL_HANDLE)
	{
		printf("failed to find a suitable GPU!\n");
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

static int	command_pool_create(t_vulk *vulk)
{
	int						queue_indice;
	VkCommandPoolCreateInfo	pool_info;

	queue_indice = vulk_queue_family(vulk->dev_phy, vulk->surface);
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

static int	command_buffer_create(t_vulk *vulk)
{
	int							i;
	uint32_t					img_count;
	VkCommandBufferAllocateInfo alloc_info;
	VkCommandBufferBeginInfo	begin_info;
	VkRenderPassBeginInfo		render_pass_info;
	VkClearValue				clear_color;

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
		printf("failed to create allocate buffer!\n");
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
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;
		vkCmdBeginRenderPass(vulk->command_buffer[i], &render_pass_info
			, VK_SUBPASS_CONTENTS_INLINE);
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

static int	semaphore_create(t_vulk *vulk)
{
	VkSemaphoreCreateInfo	semaphore_info;

	semaphore_info = (VkSemaphoreCreateInfo){};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(vulk->device, &semaphore_info, NULL
			, &vulk->semaphore_image_available) != VK_SUCCESS)
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
	if (vulk_image_view(vulk) == -1)
		return (-1);
	if (render_pass_create(vulk) == -1)
		return (-1);
	if (gpu_pipeline_create(vulk) == -1)
		return (-1);
	if (framebuffer_create(vulk) == -1)
		return (-1);
	if (command_pool_create(vulk) == -1)
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
