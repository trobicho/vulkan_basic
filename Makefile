# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/14 18:06:00 by trobicho          #+#    #+#              #
#    Updated: 2019/05/24 16:44:32 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

VULKAN_SDK_PATH = /home/tom/vulkan/1.1.106.0/x86_64
GLFW3_PATH = /home/tom/projets/glfw
GLM_PATH = /home/tom/projets/vulkan/glm

CFLAGS = -I$(VULKAN_SDK_PATH)/include -I$(GLFW3_PATH)/include -I$(GLM_PATH)
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan -lm
NAME = hello

SRCS = main.c init.c init_swap_chain.c gpu_pipeline.c shader.c pipeline_utils.c validation_layer.c

$(NAME): $(SRCS)
	gcc $(CFLAGS) $(SRCS) $(LDFLAGS) -g -o $(NAME)

clean:
	rm -f $(NAME)

re: clean $(NAME)
