/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 18:33:55 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/19 15:56:12 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "queue.h"
#include <stdlib.h>

int	get_graphic_queue_family(VkPhysicalDevice device, VkSurfaceKHR surface)
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

int	get_compute_queue_family(VkPhysicalDevice device)
{
	int						i;
	uint32_t				queue_count;
	VkQueueFamilyProperties	*queue;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, NULL);
	if (queue_count <= 0 || (queue = (VkQueueFamilyProperties*)
			malloc(sizeof(*queue) * queue_count)) == NULL)
		return (-1);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, queue);
	i = 0;
	while (i < queue_count)
	{
		if (queue[i].queueCount > 0 && queue[i].queueFlags
			& VK_QUEUE_COMPUTE_BIT)
			return (i);
		i++;
	}
	return (-1);
}
