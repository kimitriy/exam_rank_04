#ifndef MICRO_SH_H
# define MICRO_SH_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h> //for linux
# include <sys/wait.h> //for linux

# define STDIN		0
# define STDOUT		1
# define STDERR		2

# define TYPE_END	3
# define TYPE_PIPE	4
# define TYPE_BREAK	5

typedef struct		s_base
{
	char			**argv;
	int				size;
	int				type;
	int				fd[2];
	struct s_base	*prev;
	struct s_base	*next;
}					t_base;

#endif