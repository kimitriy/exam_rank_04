#ifndef MICRO_SH_H
# define MICRO_SH_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

# define STDIN		0
# define STDOUT		1
# define STDERR		2

# define TYPE_END	3
# define TYPE_PIPE	4
# define TYPE_BREAK	5

typedef struct 		s_base
{
	char			**cmnds;
	int 			size;
	int 			type;
	int 			fd[2];
	struct s_base	*prv;
	struct s_base	*nxt;
}					t_base;

#endif //MICRO_SH_H
