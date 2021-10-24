# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rburton <rburton@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/09/28 11:32:38 by rburton           #+#    #+#              #
#    Updated: 2021/06/30 04:11:17 by rburton          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	microsh

HEADER		=	micro_sh.h \

SRC 		= 	micro_sh.c \

OBJS		= ${SRC:.c=.o}

GCC			= gcc

CFLAGS		= -Wall -Wextra -Werror -g

RM			= rm -f

.PHONY: all clean re

all:		${NAME}


${OBJS}: 		${HEADER}
${NAME}:		${OBJS}
				${GCC} ${CFLAGS} -o ${NAME} ${OBJS}

clean:
		${RM} ${OBJS}

fclean:	clean
		${RM} ${NAME}
		${RM} ${LIB}

re:		fclean all
