/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   descriptor.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/13 11:56:33 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/13 12:48:48 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DESCRIPTOR_H
# define DESCRIPTOR_H

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
# include "init.h"

int	create_desc_layout(t_vulk *vulk);
int	create_desc_pool(t_vulk *vulk);
int	create_desc_set(t_vulk *vulk);
#endif
