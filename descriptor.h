/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descriptor.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/13 11:56:33 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/14 09:51:49 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DESCRIPTOR_H
# define DESCRIPTOR_H

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
# include "init.h"

int		create_desc_layout(t_vulk *vulk);
int		create_desc_pool(t_vulk *vulk);
int		create_desc_set(t_vulk *vulk);
void	update_compute_img_desc_set(VkDevice dev, VkDescriptorSet desc_set, 
	VkImageView image_view, VkImageLayout layout);
#endif