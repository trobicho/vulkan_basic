# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/14 18:06:00 by trobicho          #+#    #+#              #
#    Updated: 2019/11/08 17:47:53 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = gcc
CFLAGS = -I$(VULKAN_SDK)/include -I$(GLFW3_PATH)/include
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LDFLAGS = -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3` -lvulkan -lm
else
	LDFLAGS = -L$(GLFW3_PATH)/lib -L$(VULKAN_SDK)/lib -lvulkan -lglfw -lm
endif

NAME = basic

SRCS = main.c init.c init_swap_chain.c gpu_pipeline.c shader.c pipeline_utils.c validation_layer.c #image.c

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -g -o $(NAME)

clean:
	rm -f $(NAME)

re: clean $(NAME)
