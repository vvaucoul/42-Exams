/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 13:57:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/07/01 17:32:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

size_t	nb_args = 0;
t_arg	args[4096];
char	**renvp;

int		parse_args(size_t argc, char **argv)
{
	size_t i = 0;
	size_t current_arg = 0;


	args[current_arg].args[i] = NULL;
	args[current_arg].is_pipe = 0;

	for (size_t j = 1; j < argc; j++) {
		while (!(strcmp(argv[j], ";")) && j + 1 < argc)
		{
			++j;
		}
		if ((strcmp(argv[j], ";")) && (strcmp(argv[j], "|")))
		{
			//printf("Args[%ld][%ld] = [%s]\n", current_arg, i, argv[j]);
			args[current_arg].args[i] = ft_strdup(argv[j]);
			args[current_arg].args[i + 1] = NULL;
			++i;
		}
		else if (!(strcmp(argv[j], "|")))
		{
			args[current_arg].is_pipe = 1;
			++current_arg;
			i = 0;
			continue;
		}

		if (j + 1 < argc)
		if ((strcmp(argv[j], ";") && !strcmp(argv[j + 1], ";")))
		{
			++current_arg;
			args[current_arg].is_pipe = 0;
			i = 0;
		}
	}
	nb_args = args_len(args);
	return (0);
}

int	cd(char *dir, size_t tabsize)
{
	//printf("CD : %s\n", dir);
	if (tabsize != 2)
		return (return_error("error: cd: bad arguments", NULL, 1));
	else
		if (chdir(dir) == -1)
			return (return_error("error: cd: cannot change directory to ", dir, 1));
	return (0);
}

int	exec_cmd(char **tab)
{
	int		ret;

	if (strcmp(tab[0], "cd"))
		ret = execve(tab[0], tab, renvp);
	else
		ret = cd(tab[1], ft_tablen(tab));
	exit(ret);
	return (ret);
}

int	exec_pipe(size_t *ca)
{
	pid_t	pid;
	int		nb_pipes = 0;

	pid_t	pids[4096];
	int		ret[4096];

	int		fds[2];
	int		fd_prev;
	int		si;
	int		so;


	si = dup(STDIN_FILENO);
	so = dup(STDOUT_FILENO);


	// first
	pipe(fds);
	//printf("Exec Pipe [0] : [%s]\n", args[*ca].args[0]);
	pid = fork();
	pids[nb_pipes] = pid;
	if (!pid)
	{
		close(fds[INPUT]);
		dup2(fds[OUTPUT], STDOUT_FILENO);
		exit(exec_cmd(args[*ca].args));
	}
	close(fds[OUTPUT]);
	fd_prev = fds[INPUT];
	++(*ca);
	++nb_pipes;


	// loop

	while (*ca < nb_args && args[*ca].is_pipe)
	{
		//printf("Exec Pipe [LOOP] : [%s]\n", args[*ca].args[0]);
		pipe(fds);
		pid = fork();
		pids[nb_pipes] = pid;
		if (!pid)
		{
			close(fds[INPUT]);
			dup2(fd_prev, STDIN_FILENO);
			dup2(fds[OUTPUT], STDOUT_FILENO);
			exit(exec_cmd(args[*ca].args));
		}
		close(fds[OUTPUT]);
		fd_prev = fds[INPUT];
		++(*ca);
		++nb_pipes;
	}


	// last

	//printf("Exec Pipe [END] : [%s]\n", args[*ca].args[0]);
	pid = fork();
	pids[nb_pipes] = pid;
	if (!pid)
	{
		dup2(fd_prev, STDIN_FILENO);
		exit(exec_cmd(args[*ca].args));
	}
	close(fds[INPUT]);


	// wait

	for (size_t i = 0; i < (size_t)nb_pipes; i++) {
		waitpid(pids[i], &ret[i], 0);

		int r = 0;

		if ((r = WEXITSTATUS(ret[i])))
		{
			dup2(si, STDIN_FILENO);
			dup2(so, STDOUT_FILENO);
			close(si);
			close(so);
			return (r);
		}
	}

	dup2(si, STDIN_FILENO);
	dup2(so, STDOUT_FILENO);
	close(si);
	close(so);

	return (0);
}

int	exec_args(size_t *ca)
{
	int		st;

	if (args[*ca].is_pipe)
		return (exec_pipe(ca));

	pid_t 	pid = fork();

	if (pid < 0)
		exit_fatal("error: fatal");
	else if (!pid)
	{
		exit(exec_cmd(args[*ca].args));
	}
	else
	{
		waitpid(pid, &st, 0);
		if (WEXITSTATUS(st))
			return (WIFEXITED(st));
	}

	// cd doit s exec en dehors du fork
	if (!strcmp(args[*ca].args[0], "cd"))
		cd(args[*ca].args[1], ft_tablen(args[*ca].args));
	return (0);
}

int main(int argc, char **argv, char **envp)
{
	renvp = envp;
	if (argc < 2)
		return (0);
	parse_args(argc, argv);


	//printf("\n\n\t- NB Args = [%ld]\n", nb_args);

	for (size_t i = 0; i <= nb_args; i++) {
		for (size_t j = 0; args[i].args[j]; j++) {
			//printf("Args[%ld][%ld] = [%s]\t|\t is_pipe [%d]\n", i, j, args[i].args[j], args[i].is_pipe);
		}
	}

	//printf("\n\n\t- EXEC \n");
	for (size_t i = 0; i < nb_args; i++) {
		exec_args(&i);
	}

	return (0);
}
