/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_layer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 16:28:15 by trobicho          #+#    #+#             */
/*   Updated: 2019/05/24 16:51:54 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define GLFW_INCLUDE_VULKAN
# include "GLFW/glfw3.h"
#include "init.h"
#include <stdlib.h>

#include <string.h>

int	val_layer_check(char **val_layer, int nb_layer)
{
	int					i;
	uint32_t			a;
	uint32_t			layer_count;
	int					layer_found;
	VkLayerProperties	*available_layers;

	vkEnumerateInstanceLayerProperties(&layer_count, NULL);
	if ((available_layers = (VkLayerProperties*)
			malloc(sizeof(VkLayerProperties) * layer_count)) == NULL)
		return (0);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
	printf("layer count = %d\n", layer_count);
	i = 0;
	while (i < nb_layer)
	{
		layer_found = 0;
		while (a < layer_count && layer_found == 0)
		{
			if (strcmp(val_layer[i], available_layers[a].layerName) == 0)
				layer_found = 1;
		}
		if (!layer_found)
			return (0);
		i++;
	}
	return (1);
}
