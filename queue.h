/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 18:14:26 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/18 18:53:28 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUEUE_H
# define QUEUE_H
# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"

int	get_graphic_queue_family(VkPhysicalDevice device, VkSurfaceKHR surface);
int	get_compute_queue_family(VkPhysicalDevice device);
#endif
