#include "micro_sh.h"

//UTILS/////////////////

int	ft_strlen(char *str)
{
	int i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	err_message(char *str1, char *str2)
{
	write(STDERR, "error: ", 7);
	write(STDERR, str1, ft_strlen(str1));
	if (str2)
		write(STDERR, str2, ft_strlen(str2));
	write(STDERR, "\n", 1);
	exit(EXIT_FAILURE);
}

void	*ft_malloc(size_t count, size_t size)
{
	void	*pntr;
	pntr = (void *)malloc(count * size);
	if (NULL == pntr)
		err_message("fatal", NULL);
	return (pntr);
}

char	*ft_strdup(char *str)
{
	size_t	i;
	char	*pntr;
	if (!str)
		return (NULL);
	pntr = (char *)ft_malloc(ft_strlen(str) + 1, sizeof(char));
	i = -1;
	while (str[++i])
		pntr[i] = str[i];
	pntr[i] = '\0';
	return (pntr);
}

void	ft_lst_add_back(t_base **lst, t_base *new)
{
	t_base	*tmp;
	if (lst == NULL)
		return ;
	if (!(*lst))
	{
		*lst = new;
		new->nxt = NULL;
		new->prv = NULL;
	}
	else
	{
		tmp = *lst;
		while (tmp->nxt)
			tmp = tmp->nxt;
		tmp->nxt = new;
		new->nxt = NULL;
		new->prv = tmp;
	}
}

//PARSER////////////

int size_argv(char **argv)
{
	int i = 0;
	while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
		i++;
	return (i);
}

int check_end(char *str) //str - is an argv[i]
{
	if (!str)
		return (TYPE_END);
	if (strcmp(str, "|") == 0)
		return (TYPE_PIPE);
	if (strcmp(str, ";") == 0)
		return (TYPE_BREAK);
	return (0);
}

int		parse_argv(t_base **ptr, char **av)
{
	int		i = -1;
	int		size = size_argv(av);
	t_base	*new;
	new = (t_base *)ft_malloc(1, sizeof(t_base));
	new->cmnds = (char **)ft_malloc(size + 1, sizeof(char *));
	while (++i < size)
		new->cmnds[i] = ft_strdup(av[i]);
	new->cmnds[size] = NULL;
	new->size = size;
	new->nxt = NULL;
	new->prv = NULL;
	new->type = check_end(av[size]);
	ft_lst_add_back(ptr, new);
	return (new->size);
}

//EXECUTE//////////////

void exec_cmd(t_base *ptr, char **env)
{
	pid_t	pid;
	int		status;
	int		pipe_open = 0;
	if (ptr->type == TYPE_PIPE || (ptr->prv && ptr->prv->type == TYPE_PIPE))
	{
		pipe_open = 1;
		if (pipe(ptr->fd) < 0)
			err_message("fatal", NULL);
	}
	pid = fork();
	if (pid < 0)
		err_message("fatal", NULL);
	else if (pid == 0)
	{
		if (ptr->type == TYPE_PIPE && dup2(ptr->fd[STDOUT], STDOUT) < 0)
			err_message("fatal", NULL);
		if (ptr->prv && ptr->prv->type == TYPE_PIPE && dup2(ptr->prv->fd[STDIN], STDIN) < 0)
			err_message("fatal", NULL);
		if (execve(ptr->cmnds[0], ptr->cmnds, env) < 0)
			err_message("cannot execute ", ptr->cmnds[0]);
		exit(EXIT_SUCCESS);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (pipe_open)
		{
			close(ptr->fd[STDOUT]);
			if (!ptr->nxt || ptr->type == TYPE_BREAK)
				close(ptr->fd[STDIN]);
		}
		if (ptr->prv && ptr->prv->type == TYPE_PIPE)
			close(ptr->prv->fd[STDIN]);
	}
}

void	executor(t_base *ptr, char **env)
{
	while (ptr)
	{
		if (strcmp(ptr->cmnds[0], "cd") == 0)
		{
			if (ptr->size < 2)
				err_message("cd: bad arguments ", NULL);
			else if (chdir(ptr->cmnds[1]) < 0)
				err_message("cd: cannot change directory to ", ptr->cmnds[1]);
		}
		else
			exec_cmd(ptr, env);
		ptr = ptr->nxt;
	}
}

//MAIN//////////////////

void	free_all(t_base *ptr)
{
	t_base	*tmp;
	int		i;
	while (ptr)
	{
		tmp = ptr->nxt;
		i = 0;
		while (i < ptr->size)
		{
			free(ptr->cmnds[i]);
			i++;
		}
		free(ptr->cmnds);
		free(ptr);
		ptr = tmp;
	}
	ptr = NULL;
}

int		main(int argc, char **argv, char **env)
{
	t_base	*ptr = NULL;
	int		i = 1;
	if (argc > 1)
	{
		while (argv[i])
		{
			if (strcmp(argv[i], ";") == 0)
			{
				i++;
				continue ;
			}
			i += parse_argv(&ptr, &argv[i]);
			if (!argv[i])
				break ;
			else
				i++;
		}
		if (ptr)
			executor(ptr, env);
		free_all(ptr);
	}
	return (0);
}
