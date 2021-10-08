/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 13:59:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/10/08 14:48:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#include <stdio.h>

char **renvp;

char **find_next_pipe(char **tab)
{
    for (size_t i = 0; tab[i]; i++) {
        if (!(strcmp(tab[i], "|")))
            return (&tab[i + 1]);
    }
    return (NULL);
}

int exec_cmd(char **tab)
{
    pid_t pid = fork();
    if (pid == -1)
        return (-1);
    else if (!pid)
    {
        if ((execve(tab[0], tab, renvp)) < 0)
            return (1);
        exit (1);
    }
    return (0);
}

int execute(char **cmd)
{
    char **tmp = cmd;
    size_t nb_childs = 0;

    int fd_in = dup(STDIN_FILENO);

    while (tmp)
    {
        int
    }

    for (size_t i = nb_childs; i > 0; i++) {
        waitpid(0, NULL, 0);
    }

    return (0);
}

int main(int argc, char **argv, char **envp) {
    char *args[4096];

    (void)argc;
    (void)argv;

    renvp = envp;
    if (!(args = malloc(sizeof(*args) * 4096)))
        return (1);

    args[0] = "/bin/ls";
    args[1] = "|";
    args[2] = "/usr/bin/grep";
    args[3] = "c";
    args[4] = NULL;

    execute(args);

    return 0;
}
