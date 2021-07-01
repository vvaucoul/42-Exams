/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 14:04:30 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/07/01 16:57:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

void	ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
}

int		exit_fatal(char *str)
{
	ft_putstr_fd(str, 2);
	ft_putstr_fd("\n", 2);
	exit(1);
}

int		return_error(char *str, char *suffix, int err)
{
	if (suffix)
	{
		ft_putstr_fd(str, 2);
		ft_putstr_fd(suffix, 2);
		ft_putstr_fd("\n", 2);
	}
	else
	{
		ft_putstr_fd(str, 2);
		ft_putstr_fd("\n", 2);
	}
	return(err);
}

size_t	ft_strlen(char *str)
{
	size_t i = 0;

	while (str[i])
		++i;
	return (i);
}

char *ft_strdup(char *str)
{
	char *nstr;
	size_t i = 0;

	if (!(nstr = (char *)malloc(sizeof(char) * ft_strlen(str) + 1)))
		return (NULL);
	for (; str[i]; i++)
		nstr[i] = str[i];
	nstr[i] = 0;
	return (nstr);
}

size_t	ft_tablen(char **tab)
{
	size_t i = 0;

	while (tab[i])
		++i;
	return (i);
}

size_t	ft_tab_occur(char **tab, char *str)
{
	size_t i = 0;
	size_t nb = 0;

	while (tab[i])
	{
		if (!(strcmp(tab[i], str)))
			++nb;
		++i;
	}
	return (nb);
}

size_t	args_len(t_arg *args)
{
	size_t i = 0;

	while (args[i].args[0])
		++i;
	return (i);
}
