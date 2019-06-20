/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descriptor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/13 11:57:49 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/20 15:02:17 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "init.h"

int	create_desc_layout(t_vulk *vulk)
{
	VkDescriptorSetLayoutCreateInfo		layout_info = {};
	static VkDescriptorSetLayoutBinding	layout_binding = {};
	
	layout_binding.binding = 0;
	layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	layout_binding.descriptorCount = 1;
	layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	layout_binding.pImmutableSamplers = NULL;
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 1;
	layout_info.pBindings = &layout_binding;
	if (vkCreateDescriptorSetLayout(vulk->device, &layout_info, NULL
			, &vulk->compute.desc_set_layout_pre) != VK_SUCCESS)
	{
		printf("failed to create compute descriptor layout\n");
		return (-1);
	}
	layout_binding.binding = 0;
	layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layout_binding.descriptorCount = 1;
	layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layout_binding.pImmutableSamplers = NULL;
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 1;
	layout_info.pBindings = &layout_binding;
	if (vkCreateDescriptorSetLayout(vulk->device, &layout_info, NULL
			, &vulk->compute.desc_set_layout_post) != VK_SUCCESS)
	{
		printf("failed to create fragment descriptor layout\n");
		return (-1);
	}
	return (0);
}

int	create_desc_pool(t_vulk *vulk)
{
	VkDescriptorPoolSize		pool_size[2];
	VkDescriptorPoolCreateInfo	pool_info;

	pool_info = (VkDescriptorPoolCreateInfo){};
	pool_size[0] = (VkDescriptorPoolSize){};
	pool_size[0].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	pool_size[0].descriptorCount = 1;
	pool_size[1] = (VkDescriptorPoolSize){};
	pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_size[1].descriptorCount = 1;
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 2;
	pool_info.pPoolSizes = pool_size;
	pool_info.maxSets = 2;
	if (vkCreateDescriptorPool(vulk->device, &pool_info, NULL
			, &vulk->desc_pool) != VK_SUCCESS)
	{
		printf("failed to create descriptor pool\n");
		return (-1);
	}

	return (0);
}

int	create_desc_set(t_vulk *vulk)
{
	int							i;
	VkDescriptorSetAllocateInfo	alloc_info;
	VkDescriptorBufferInfo		buffer_info;

	buffer_info = (VkDescriptorBufferInfo){};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = vulk->desc_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &vulk->compute.desc_set_layout_pre;
	if (vkAllocateDescriptorSets(vulk->device, &alloc_info
			, &vulk->compute.desc_set_pre) != VK_SUCCESS)
	{
		printf("failed to allocate descriptor sets!\n");
		return (-1);
	}
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = vulk->desc_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &vulk->compute.desc_set_layout_post;
	if (vkAllocateDescriptorSets(vulk->device, &alloc_info
			, &vulk->compute.desc_set_post) != VK_SUCCESS)
	{
		printf("failed to allocate descriptor sets!\n");
		return (-1);
	}
	return (0);
}

void	update_desc_set(t_vulk *vulk)
{
	VkWriteDescriptorSet	desc_write[2];

	desc_write[0] = (VkWriteDescriptorSet){};
	desc_write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write[0].dstSet = vulk->compute.desc_set_pre;
	desc_write[0].dstBinding = 0;
	desc_write[0].dstArrayElement = 0;
	desc_write[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	desc_write[0].descriptorCount = 1;
	desc_write[0].pImageInfo = &vulk->compute.img_info;
	desc_write[1] = (VkWriteDescriptorSet){};
	desc_write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write[1].dstSet = vulk->compute.desc_set_post;
	desc_write[1].dstBinding = 0;
	desc_write[1].dstArrayElement = 0;
	desc_write[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	desc_write[1].descriptorCount = 1;
	desc_write[1].pImageInfo = &vulk->compute.img_info;
	vkUpdateDescriptorSets(vulk->device, 2, &desc_write, 0, NULL);
}

void	update_compute_img_desc_set(VkDevice dev, VkDescriptorSet desc_set,
	VkImageView image_view, VkImageLayout layout)
{
	VkWriteDescriptorSet	desc_write;
	VkDescriptorImageInfo	img_info;

	img_info.sampler = VK_NULL_HANDLE;
	img_info.imageView = image_view;
	img_info.imageLayout = layout;

	desc_write = (VkWriteDescriptorSet){};
	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = desc_set;
	desc_write.dstBinding = 0;
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	desc_write.descriptorCount = 1;
	desc_write.pImageInfo = &img_info;
	vkUpdateDescriptorSets(dev, 1, &desc_write, 0, NULL);
}

void	update_compute_post_img_desc_set(VkDevice dev, VkDescriptorSet desc_set,
	VkImageView image_view, VkImageLayout layout)
{
	VkWriteDescriptorSet	desc_write;
	VkDescriptorImageInfo	img_info;

	img_info.sampler = VK_NULL_HANDLE;
	img_info.imageView = image_view;
	img_info.imageLayout = layout;

	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = desc_set;
	desc_write.dstBinding = 0;
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	desc_write.descriptorCount = 1;
	desc_write.pImageInfo = &img_info;
	vkUpdateDescriptorSets(dev, 1, &desc_write, 0, NULL);
}
