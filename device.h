/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 19:39:00 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/18 19:43:29 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEVICE_H
# define DEVICE_H
# include "init.h"

int	vulk_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
int	vulk_physical_device(t_vulk *vulk);
int	vulk_logical_device(t_vulk *vulk);
#endif
