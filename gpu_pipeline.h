/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gpu_pipeline.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/22 17:58:06 by trobicho          #+#    #+#             */
/*   Updated: 2019/05/23 18:46:01 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GPU_PIPELINE
# define GPU_PIPELINE

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
# include "init.h"

int	gpu_pipeline_create(t_vulk *vulk);
#endif
