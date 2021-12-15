/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 13:40:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/12/15 16:43:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

# include <stdio.h>

typedef struct s_client {
	struct s_client *next;
	int fd;
	int id;
}	t_client;

t_client *clients; // List clients
int		sockfd = 0, max_id = 0; // srv socket FD | max id
fd_set	current_socket, sock_read, sock_write; // current socket, read, write
char	msg[42];
char	str[42 * 4096], tmp[42 * 4096], buf[42 * 4096 + 42];

// UTILS //

// Exit fatal
// Fatal error && close sockfd
void 	exit_fatal()
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close(sockfd);
	exit(1);
}
// return max fd from all clients
int get_max_fd()
{
	int max_fd = sockfd;
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd > max_fd)
			max_fd = tmp->fd;
		tmp = tmp->next;
	}
	return (max_fd);
}

// Send msg to all except sender
// check if fd != client fd && client fd is set to sock_write
// send to client fd, str
void send_all(int fd, char *str)
{
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd != fd && FD_ISSET(tmp->fd, &sock_write))
		{
			if (send(tmp->fd, str, strlen(str), 0) < 0)
				exit_fatal();
		}
		tmp = tmp->next;
	}
}

// Add client to end of the list
// Init new client -> calloc & init ID FD & next to NULL
// add new to end of the clients
// if clients == NULL clients = new;
int	 add_client_to_list(int fd)
{
	t_client *tmp = clients;
	t_client *new;

	if (!(new = calloc(1, sizeof(t_client))))
		exit_fatal();
	new->id = max_id;
	++max_id;
	new->fd = fd;
	new->next = NULL;
	if (!clients)
		clients = new;
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
	return (new->id);
}

// Add client
// accept new client
// add client to list & send all
void add_client()
{
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int client_fd;

	if ((client_fd = accept(sockfd, (struct sockaddr *) &clientaddr, &len)) < 0)
		exit_fatal();
	bzero(&msg, sizeof(msg));
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd));
	send_all(client_fd, msg);
	FD_SET(client_fd, &current_socket);
}

// Get id from clients, compare with fd
int get_id(int fd)
{
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp->fd == fd)
			return (tmp->id);
		tmp = tmp->next;
	}
	return (-1);
}

int remove_client(int fd)
{
	t_client *tmp = clients;
	t_client *to_delete;

	int id = get_id(fd);

	if (tmp && tmp->fd == fd)
	{
		clients = tmp->next;
		free(tmp);
	}
	else
	{
		while (tmp && tmp->next && tmp->next->fd != fd)
			tmp = tmp->next;
		to_delete = tmp->next;
		tmp->next = tmp->next->next;
		free(to_delete);
	}
	return (id);
}

// send msg to clients
// truncate string at each \n
void ex_msg(int fd)
{
	int i = 0;
	int j = 0;

	while (str[i])
	{
		tmp[j] = str[i];
		j++;
		if (str[i] == '\n')
		{
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, strlen(tmp));
			bzero(&buf, strlen(buf));
		}
		i++;
	}
	bzero(&str, strlen(str));
}

// LOOP //

int loop_server()
{
	while (1)
	{
		sock_write = sock_read = current_socket;
		if (select(get_max_fd() + 1, &sock_read, &sock_write, NULL, NULL) < 0)
			continue;
		for (int fd = 0; fd <= get_max_fd(); fd++) {
			if (FD_ISSET(fd, &sock_read))
			{
				if (fd == sockfd)
				{
					add_client();
					break;
				}
				else
				{
					if (recv(fd, str, sizeof(str), 0) <= 0)
					{
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", remove_client(fd));
						send_all(fd, msg);
						FD_CLR(fd, &current_socket);
						close(fd);
						break;
					}
					else
						ex_msg(fd);
				}
			}
		}
	}
}

// INIT //

int init_values()
{
	FD_ZERO(&current_socket);
	FD_SET(sockfd, &current_socket);
	bzero(&tmp, sizeof(tmp));
	bzero(&buf, sizeof(buf));
	bzero(&str, sizeof(str));
	return (0);
}

int init_server(uint16_t port)
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_fatal();
	if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		exit_fatal();
	if (listen(sockfd, 0) < 0)
		exit_fatal();
	return (0);
}

// MAIN //

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	init_server(atoi(argv[1]));
	init_values();
	loop_server();
	return (0);
}
