#include "micro_sh.h"

//UTILS//5

int     ft_strlen(char *str)
{
	int i = 0;
	if (str == NULL)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void    err_message(char *str1, char *str2)
{
	write(STDERR, "error: ", 7);
	write(STDERR, str1, ft_strlen(str1));
	if (str2)
		write(STDERR, str2, ft_strlen(str2));
	write(STDERR, "\n", 1);
	exit(EXIT_FAILURE);
}

void    *ft_malloc(size_t count, size_t size)
{
	void *pntr;
	pntr = (void *)malloc(count * size);
	if (NULL == pntr)
		err_message("fatal", NULL);
	return (pntr);
}

char    *ft_strdup(char *str)
{

}

void    ft_lst_add_back(t_base **lst, t_base *new)
{

}


//PRSR//3

int     size_argv(char **argv)
{

}

int     check_end(char *str)
{

}

int     parser_argv(t_base **ptr, char **av)
{

}


//EXEC//2

void    exec_cmd(t_base *tmp, char **env)
{

}

void    executor(t_base *ptr, char **env)
{

}


//MAIN//2
void    free_all(t_base *ptr)
{

}

int     main(int argc, char **argv, char **env)
{
	return (0);
}