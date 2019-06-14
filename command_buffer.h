/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_buffer.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobicho <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/14 08:48:28 by trobicho          #+#    #+#             */
/*   Updated: 2019/06/14 09:32:56 by trobicho         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_BUFFER
# define COMMAND_BUFFER
# include "init.h"

int	compute_command_buffer_create(t_vulk *vulk);
int	command_buffer_create(t_vulk *vulk);
#endif
