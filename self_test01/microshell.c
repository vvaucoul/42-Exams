#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

typedef struct	s_arg
{
	char	*tab[4096];
	int		is_pipe;
}		t_arg;


t_arg	args[4096];
size_t	nb_args = 0;
char	**renvp;

size_t  ft_strlen(char *str);

/*
**	UTILS
*/

void	ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
}

int	exit_fatal()
{
	ft_putstr_fd("error fatal\n", 2);
	exit(1);
}

int     display_error(char *str, char *suffix, int error)
{
	if (!suffix)
	{
		ft_putstr_fd(str, 2);
		ft_putstr_fd("\n", 2);
	}
	else
	{
		ft_putstr_fd(str, 2);
		ft_putstr_fd(suffix, 2);
		ft_putstr_fd("\n", 2);
	}
	return (error);
}

size_t 	ft_strlen(char *str)
{
	size_t i = 0;

	while (str[i])
	++i;
	return (i);
}

char	*ft_strdup(char *str)
{
	size_t i = 0;
	char	*nstr;

	if (!(nstr = malloc(sizeof(char) * (ft_strlen(str) + 1))))
	exit_fatal();

	while (str[i])
	{
		nstr[i] = str[i];
		++i;
	}
	nstr[i] = 0;
	return (nstr);
}

size_t get_arg_len()
{
	size_t i = 0;

	while (args[i].tab[0])
	++i;
	return (i);
}

size_t	ft_tablen(char **tab)
{
	size_t i = 0;

	while (tab[i])
		++i;
	return (i);
}

/*
**	PARSING
*/

int	parse_args(int argc, char **argv)
{
	size_t	actu_cmd = 0;
	size_t	j = 0;
	size_t	i = 1;

	while (i < (size_t)argc)
	{
		if (strcmp(argv[i], ";") && strcmp(argv[i], "|"))
		{
			args[actu_cmd].tab[j] = ft_strdup(argv[i]);
			args[actu_cmd].tab[j + 1] = NULL;
			++j;
		}
		else if (!(strcmp(argv[i], ";")))
		{
			while (!(strcmp(argv[i], ";")) && argv[i + 1])
				++i;
			if (args[actu_cmd].tab[0])
			{
				actu_cmd++;
				j = 0;
			}
			continue ;
		}
		else if (!(strcmp(argv[i], "|")))
		{
			args[actu_cmd].is_pipe = 1;
			actu_cmd++;
			j = 0;
		}
		++i;
	}
	return (0);
}

/*
**	EXECUTE
*/

int	cd(char *dir, size_t nb_args)
{
	if (nb_args != 2)
		return (display_error("error: cd: bad arguments", NULL, 1));
	else
	{
		if ((chdir(dir)) < 0)
			return (display_error("error: cd: cannot change directory to ", dir, 1));
	}
	return (0);
}

int	exec_tab(char **tab)
{
	int ret = 0;

	ret = execve(tab[0], tab, renvp);
	if (ret < 0)
		exit (display_error("error: cannot execute ", tab[0], -1));
	exit(ret);
}

int	multi_pipes(size_t *i)
{
	int si = dup(STDIN_FILENO);
	int so = dup(STDOUT_FILENO);

	size_t nb_pipes = 0;
	int fds[2];
	int fd_prev = 0;

	int	pids[4096];
	int	st[4096];

	pid_t pid;



	// first

	pipe(fds);
	pid = fork();
	pids[nb_pipes] = pid;
	if (pid < 0)
		exit_fatal();
	else if (!pid)
	{
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		exit(exec_tab(args[*i].tab));
	}
	close(fds[1]);
	fd_prev = fds[0];
	++(*i);
	++nb_pipes;

	// loop

	while (args[(*i)].is_pipe && (*i + 1 < nb_args))
	{
		pipe(fds);
		pid = fork();
		pids[nb_pipes] = pid;
		if (pid < 0)
			exit_fatal();
		else if (!pid)
		{
			close(fds[0]);
			dup2(fd_prev, STDIN_FILENO);
			dup2(fds[1], STDOUT_FILENO);
			exit(exec_tab(args[*i].tab));
		}
		close(fds[1]);
		fd_prev = fds[0];
		++nb_pipes;
		++(*i);
	}

	// last

	pid = fork();
	pids[nb_pipes] = pid;
	if (pid < 0)
		exit_fatal();
	else if (!pid)
	{
		dup2(fds[0], STDIN_FILENO);
		exit(exec_tab(args[*i].tab));
	}
	close(fds[0]);

	size_t n = 0;
	while (n < nb_pipes)
	{
		int ret = 0;

		waitpid(pids[n], &st[n], 0);
		if ((ret = WEXITSTATUS(st[n])))
		{
			dup2(si, STDIN_FILENO);
			dup2(so, STDOUT_FILENO);
			return (ret);
		}
		++n;
	}

	dup2(si, STDIN_FILENO);
	dup2(so, STDOUT_FILENO);
	return (0);
}

int	execute_args(size_t *i)
{
	pid_t	pid;
	int		st;


	if (!(strcmp(args[*i].tab[0], "cd")))
		return (cd(args[*i].tab[1], ft_tablen(args[*i].tab)));
	else if (args[*i].is_pipe == 1)
		return (multi_pipes(i));
	pid = fork();
	if (pid < 0)
		exit_fatal();
	else if (!pid)
	{
		exit(exec_tab(args[*i].tab));
	}
	else
	{
		int ret = 0;
		waitpid(pid, &st, 0);
		if ((ret = WEXITSTATUS(st)))
			return (ret);
	}
	return (0);
}

/*
**	END
*/

int	free_args()
{
	size_t i = 0;
	size_t j = 0;


	while (i < nb_args)
	{
		while (args[i].tab[j])
		{
			free(args[i].tab[j]);
			++j;
		}
		++i;
		j = 0;
	}
	return (0);
}

int main(int argc, char **argv, char **envp)
{
	if (argc <= 1)
		return (0);
	renvp = envp;
	parse_args(argc, argv);
	nb_args = get_arg_len();

	//printf("NB ARGS = [%ld]\n", nb_args);
	//for (size_t i = 0; i < (size_t)argc - 1; i++)
	//{
	//	for (size_t j = 0; args[i].tab[j]; j++)
	//	printf("tab[%ld][%ld] = [%s] \r\t\t\t\t|\tIS Pipe [%d]\n", i, j, args[i].tab[j], args[i].is_pipe);
	//}

	//printf("\n\n\n## EXECUTE ##\n\n\n");

	size_t i = 0;
	while (i < nb_args)
	{
		execute_args(&i);
		++i;
	}

	free_args();
	return (0);
}
