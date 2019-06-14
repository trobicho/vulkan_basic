# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/14 18:06:00 by trobicho          #+#    #+#              #
#    Updated: 2019/06/14 08:49:23 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

VULKAN_SDK_PATH = /Volumes/Storage/goinfre/trobicho/vulkansdk-macos-1.1.106.0/macOS
GLFW3_PATH = /Users/trobicho/.brew/Cellar/glfw/HEAD-d834f01

CFLAGS = -I$(VULKAN_SDK_PATH)/include -I$(GLFW3_PATH)/include -g
LDFLAGS = -L$(GLFW3_PATH)/lib -L$(VULKAN_SDK_PATH)/lib -lvulkan -lglfw -lm
NAME = hello

SRCS = main.c init.c init_swap_chain.c gpu_pipeline.c shader.c pipeline_utils.c command_buffer.c descriptor.c #image.c

$(NAME): $(SRCS)
	gcc $(CFLAGS) $(LDFLAGS) $(SRCS) -g -o $(NAME)

clean:
	rm -f $(NAME)

re: clean $(NAME)
