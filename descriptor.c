/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descriptor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/13 11:57:49 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/13 16:47:58 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	uint32_t					img_count;
	VkDescriptorPoolSize		pool_size[2];
	VkDescriptorPoolCreateInfo	pool_info;

	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	pool_info = (VkDescriptorPoolCreateInfo){};
	pool_size[0] = (VkDescriptorPoolSize){};
	pool_size[0].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	pool_size[0].descriptorCount = 1; //idk
	pool_size[1] = (VkDescriptorPoolSize){};
	pool_size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_size[1].descriptorCount = 1; //idk
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
	uint32_t					img_count;
	VkDescriptorSetAllocateInfo	alloc_info_pre;
	VkDescriptorSetAllocateInfo	alloc_info_post;
	VkDescriptorSetLayout		layout;
	VkDescriptorBufferInfo		buffer_info;
	VkWriteDescriptorSet		desc_write;

	buffer_info = (VkDescriptorBufferInfo){};
	desc_write = (VkWriteDescriptorSet){};
	vkGetSwapchainImagesKHR(vulk->device, vulk->swap_chain, &img_count, NULL);
	alloc_info_pre.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info_pre.descriptorPool = vulk->desc_pool;
	alloc_info_pre.descriptorSetCount = 1;
	alloc_info_pre.pSetLayouts = &vulk->compute.desc_set_layout_pre;
	alloc_info_post.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info_post.descriptorPool = vulk->desc_pool;
	alloc_info_post.descriptorSetCount = 1;
	alloc_info_post.pSetLayouts = &vulk->compute.desc_set_layout_post;
	if (vkAllocateDescriptorSets(vulk->device, &alloc_info_pre
			, &vulk->compute.desc_set_pre) != VK_SUCCESS
		|| vkAllocateDescriptorSets(vulk->device, &alloc_info_post
			, &vulk->compute.desc_set_post) != VK_SUCCESS)
	{
		printf("failed to allocate descriptor sets!\n");
		return (-1);
	}
	vulk->compute.img_desc.sampler = 
	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = vulk->compute.desc_set_pre;
	desc_write.dstBinding = 0;
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	desc_write.descriptorCount = 1;
	desc_write.pImageInfo = &vulk->compute.img_desc;
	vkUpdateDescriptorSets(vulk->device, 1, &desc_write, 0, NULL);
	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	desc_write.dstSet = vulk->compute.desc_set_post;
	desc_write.dstBinding = 0;
	desc_write.dstArrayElement = 0;
	desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	desc_write.descriptorCount = 1;
	desc_write.pImageInfo = &vulk->compute.img_desc;
	vkUpdateDescriptorSets(vulk->device, 1, &desc_write, 0, NULL);
	free(layout);
	return (0);
}
