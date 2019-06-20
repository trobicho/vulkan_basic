# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: trobicho <trobicho@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/14 18:06:00 by trobicho          #+#    #+#              #
#    Updated: 2019/06/20 02:26:32 by trobicho         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = clang
CFLAGS = -I$(VULKAN_SDK_PATH)/include -I$(GLFW3_PATH)/include
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan -lm
else
	LDFLAGS = -L$(GLFW3_PATH)/lib -L$(VULKAN_SDK_PATH)/lib -lvulkan -lglfw -lm
endif

NAME = hello

SRCS = main.c init.c validation_layer.c device.c init_swap_chain.c queue.c descriptor.c command_buffer.c \
	   gpu_pipeline.c pipeline_utils.c shader.c image.c\

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -g -o $(NAME)

clean:
	rm -f $(NAME)

re: clean $(NAME)
