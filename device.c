/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 19:39:03 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/19 15:56:43 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "init.h"
#include "init_swap_chain.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int	vulk_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VkPhysicalDeviceProperties	dev_prop;
	VkPhysicalDeviceFeatures	dev_feat;
	t_swap_chain_detail			detail;

	vkGetPhysicalDeviceProperties(device, &dev_prop);
	vkGetPhysicalDeviceFeatures(device, &dev_feat);
	if (get_graphic_queue_family(device, surface) == -1)
		return (0);
	detail = swap_get_detail(device, surface);
	if (detail.format == NULL || detail.present_mode == NULL)
		return (0);
	return (1);
}

int	vulk_physical_device(t_vulk *vulk)
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

int	vulk_logical_device(t_vulk *vulk)
{
	int							dev_i;
	float						queue_priority;
	VkDeviceCreateInfo			dev_inf = {};
	VkDeviceQueueCreateInfo		queue_inf = {};
	VkPhysicalDeviceFeatures	dev_feat = {};
	char						*ext_name;

	ext_name = strdup(VK_KHR_SWAPCHAIN_EXTENSION_NAME); //libc!! free??
	if ((dev_i = get_graphic_queue_family(vulk->dev_phy, vulk->surface)) == -1)
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
