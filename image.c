/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/10 09:23:21 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/20 14:12:04 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init.h"
#include <stdio.h>
#include <stdlib.h>

int		create_image(t_vulk * vulk, VkFormat format)
{
	VkFormatProperties		format_prop;
	VkImageCreateInfo		image_create_info;
	VkMemoryAllocateInfo	mem_alloc_info;
	VkMemoryRequirements	mem_req;
	VkSamplerCreateInfo		sampler;
	VkImageViewCreateInfo	view_info;

	vulk->compute.img_format = format;
	vkGetPhysicalDeviceFormatProperties(vulk->dev_phy, format, &format_prop);
	if ((format_prop.optimalTilingFeatures
		& VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == 0)
	{
		printf("incompatible format!!!\n");
		return (-1);
	}
	image_create_info = (VkImageCreateInfo){};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.format = format;
	image_create_info.extent = (VkExtent3D){vulk->swap_chain_extent.width
		, vulk->swap_chain_extent.height, 1};
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT
		| VK_IMAGE_USAGE_STORAGE_BIT;
	image_create_info.flags = 0;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vulk->device, &image_create_info, NULL
			, &vulk->compute.img) != VK_SUCCESS)
	{
		printf("failed to create image!\n");
		return (-1);
	}

	vkGetImageMemoryRequirements(vulk->device, vulk->compute.img, &mem_req);
	mem_alloc_info = (VkMemoryAllocateInfo){};
	mem_alloc_info.allocationSize = mem_req.size;
	mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc_info.memoryTypeIndex = find_memory_type(vulk
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mem_req.memoryTypeBits);
	if (vkAllocateMemory(vulk->device, &mem_alloc_info, NULL
			, &vulk->compute.img_mem) != VK_SUCCESS)
	{
		printf("failed to allocate memory for the image!\n");
		return (-1);
	}
	if (vkBindImageMemory(vulk->device, vulk->compute.img
			, vulk->compute.img_mem, 0) != VK_SUCCESS)
	{
		printf("failed to bind memory for the image!\n");
		return (-1);
	}

	sampler = (VkSamplerCreateInfo){};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	sampler.addressModeV = sampler.addressModeU;
	sampler.addressModeW = sampler.addressModeU;
	sampler.mipLodBias = 0.0;
	sampler.maxAnisotropy = 1.0;
	sampler.compareOp = VK_COMPARE_OP_NEVER;
	sampler.minLod = 0.0;
	sampler.maxLod = 1.0;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	if (vkCreateSampler(vulk->device, &sampler, NULL, &vulk->compute.sampler)
		!= VK_SUCCESS)
	{
		printf("failed to create sampler!\n");
		return (-1);
	}

	view_info = (VkImageViewCreateInfo){};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = vulk->compute.img;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
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
				, &vulk->compute.image_view) != VK_SUCCESS)
	{
		printf("failed to create image views!");
		return (-1);
	}
	vulk->compute.img_info.sampler = vulk->compute.sampler;
	vulk->compute.img_info.imageView = vulk->compute.image_view;
	vulk->compute.img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	return (0);
}
