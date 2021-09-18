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

void	ft_lstadd_back(t_base **lst, t_base *new)
{
	t_base	*tmp;
	if (lst == NULL)
		return ;
	if (!(*lst))
	{
		*lst = new;
		new->next = NULL;
		new->prev = NULL;
	}
	else
	{
		tmp = *lst;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
		new->next = NULL;
		new->prev = tmp;
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

int		parser_argv(t_base **ptr, char **av)
{
	int		i = -1;
	int		size = size_argv(av);
	t_base	*new;
	new = (t_base *)ft_malloc(1, sizeof(t_base));
	new->argv = (char **)ft_malloc(size + 1, sizeof(char *));
	new->argv[size] = NULL;
	new->size = size;
	new->next = NULL;
	new->prev = NULL;
	while (++i < size)
		new->argv[i] = ft_strdup(av[i]);
	new->type = check_end(av[size]);
	ft_lstadd_back(ptr, new);
	return (new->size);
}

//EXECUTE//////////////

void exec_cmd(t_base *tmp, char **env)
{
	pid_t pid;
	int status;
	int pipe_open;
	pipe_open = 0;
	if (tmp->type == TYPE_PIPE || (tmp->prev && tmp->prev->type == TYPE_PIPE))
	{
		pipe_open = 1;
		if (pipe(tmp->fd) < 0) //if pipe creation fails
			err_message("fatal", NULL);
	}
	pid = fork();
	if (pid < 0) //if fork() fails
		err_message("fatal", NULL);
	else if (pid == 0) //child
	{
		if (tmp->type == TYPE_PIPE && dup2(tmp->fd[STDOUT], STDOUT) < 0)
			err_message("fatal", NULL);
		if (tmp->prev && tmp->prev->type == TYPE_PIPE && dup2(tmp->prev->fd[STDIN], STDIN) < 0)
			err_message("fatal", NULL);
		if ((execve(tmp->argv[0], tmp->argv, env)) < 0)
			err_message("cannot execute ", tmp->argv[0]);
		exit(EXIT_SUCCESS);
	}
	else //parent
	{
		waitpid(pid, &status, 0);
		if (pipe_open)
		{
			close(tmp->fd[STDOUT]);
			if (!tmp->next || tmp->type == TYPE_BREAK)
				close(tmp->fd[STDIN]);
		}
		if (tmp->prev && tmp->prev->type == TYPE_PIPE)
			close(tmp->prev->fd[STDIN]);
	}
}

void executor(t_base *ptr, char **env)
{
	t_base *tmp;
	tmp = ptr;
	while (tmp)
	{
		if (strcmp("cd", tmp->argv[0]) == 0) //if cd
		{
			if (tmp->size < 2)
				err_message("cd: bad arguments ", NULL);
			else if (chdir(tmp->argv[1]) < 0)
			{
				// write(1, "cd\n", 3);
				// write(1, tmp->argv[1], ft_strlen(tmp->argv[1]));
				// write(1, "\n", 1);
				err_message("cd: cannot change directory to ", tmp->argv[1]);
			}	
		}
		else
			exec_cmd(tmp, env);
		tmp = tmp->next;
	}
}

//MAIN//////////////////

void	free_all(t_base *ptr)
{
	t_base	*tmp;
	int		i;
	while (ptr)
	{
		tmp = ptr->next;
		i = 0;
		while (i < ptr->size)
		{
			free(ptr->argv[i]);
			i++;
		}
		free(ptr->argv);
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
			i += parser_argv(&ptr, &argv[i]);
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
