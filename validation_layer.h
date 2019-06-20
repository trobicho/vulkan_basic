/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_layer.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 16:27:58 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/19 19:28:34 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATION_LAYER
# define VALIDATION_LAYER
# include "init.h"

int	val_layer_check(char **val_layer, int nb_layer);
char**	get_extensions(int debug, uint32_t *ext_count
	, int val_layer_count, char **val_layer);
#endif
