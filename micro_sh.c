#include "micro_sh.h"

//UTILS//5

int		ft_strlen(char *str)
{
	int		i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	err_message(char *msg1, char *msg2)
{
	write(STDERR, "error: ", 7);
	write(STDERR, msg1, ft_strlen(msg1));
	if (msg2)
		write(STDERR, msg2, ft_strlen(msg2));
	write(STDERR, "\n", 1);
	exit(EXIT_FAILURE);
}

void	*ft_malloc(size_t count, size_t size)
{
	void	*ptr;
	ptr = (void *)malloc(count * size);
	if (NULL == ptr)
		err_message("fatal", NULL);
	return (ptr);
}

char	*ft_strdup(char *str)
{
	int		i;
	char	*ptr;
	if (!str)
		return (NULL);
	ptr = (char *)ft_malloc(ft_strlen(str) + 1, sizeof(char));
	i = -1;
	while (str[++i])
		ptr[i] = str[i];
	ptr[i] = '\0';
	return (ptr);
}

void	ft_lst_add_back(t_base **lst, t_base *new)
{
	t_base	*tmp;
	if (!lst)
		return ;
	if (!(*lst))
	{
		*lst = new;
		new->prv = NULL;
		new->nxt = NULL;
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

//PRSR//3

int		size_argv(char **av)
{
	int		i = 0;
	while (av[i] && strcmp(av[i], "|") != 0 && strcmp(av[i], ";") != 0)
		i++;
	return (i);
}

int		check_end(char *str)
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
	t_base	*new;
	int		size = size_argv(av);
	int		i = -1;
	new = (t_base *)ft_malloc(1, sizeof(t_base));
	new->cmnds = (char **)ft_malloc(size + 1, sizeof(char *));
	while (++i < size)
		new->cmnds[i] = ft_strdup(av[i]);
	new->cmnds[size] = NULL;
	new->size = size;
	new->type = check_end(av[size]);
	new->prv = NULL;
	new->nxt = NULL;
	ft_lst_add_back(ptr, new);
	return (new->size);
}

//EXEC//2

void	exec_cmd(t_base *ptr, char **env)
{
	pid_t	pid;
	int		status;
	int		pipe_open = 0;
	if (ptr->type == TYPE_PIPE || (ptr->prv && ptr->prv->type == TYPE_PIPE))
	{
		if (pipe(ptr->fd) < 0)
			err_message("fatal", NULL);
		pipe_open = 1;
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

void	execute(t_base *ptr, char **env)
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

//MAIN//2

void	free_all(t_base *ptr)
{
	t_base	*tmp;
	int		i;
	while (ptr)
	{
		tmp = ptr->nxt;
		i = -1;
		while (++i < ptr->size)
			free(ptr->cmnds[i]);
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
				continue;
			}
			i += parse_argv(&ptr, &argv[i]);
			if (!argv[i])
				break;
			else
				i++;
		}
		if (ptr)
			execute(ptr, env);
		free_all(ptr);
	}
	return (0);
}