/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 11:48:52 by trobicho          #+#    #+#             */
/*   Updated: 2019/05/23 19:01:27 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_H
# define SHADER_H
# include "init.h"

# define BUFF_SIZE	2048 

char	*shader_load(char* file, int *size_ret);
int		shader_create_module(t_vulk *vulk, const char *bytecode
	, int size, VkShaderModule *shader_module);
#endif
