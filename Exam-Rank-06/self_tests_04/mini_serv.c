#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct s_client
{
	int id;
	int fd;
	struct s_client *next;
}	t_client;

int g_id = 0;
t_client *clients = NULL;
int sockfd = -1;
char msg[42];
fd_set current_socket, sock_read, sock_write;

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
	len = 0;
	else
	len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void exit_fatal()
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	if (sockfd != -1)
		close(sockfd);
	exit(1);
}

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

int get_max_fd()
{
	t_client *tmp = clients;
	int max = sockfd;

	while (tmp)
	{
		if (tmp->fd > max)
			max = tmp->fd;
		tmp = tmp->next;
	}
	return (max);
}

int add_client_to_list(int fd)
{
	t_client *tmp = clients;
	t_client *new = NULL;

	if (!(new = calloc(1, sizeof(t_client))))
		exit_fatal();
	new->fd = fd;
	new->id = g_id++;
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

void send_all(int fd, char *str)
{
	t_client *tmp = clients;

	while (tmp)
	{
		if (tmp && tmp->fd != fd && FD_ISSET(tmp->fd, &sock_write))
		{
			if ((send(tmp->fd, str, strlen(str), 0)) < 0)
				exit_fatal();
		}
		tmp = tmp->next;
	}
}

void add_client()
{
	struct sockaddr clientaddr;
	socklen_t len = sizeof(clientaddr);
	int client_fd;

	if ((client_fd = accept(sockfd, (struct sockaddr *) &clientaddr, &len)) < 0)
		exit_fatal();

	bzero(&msg, sizeof(msg));
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd));
	send_all(client_fd, msg);
	FD_SET(client_fd, &current_socket);
}

int remove_client(int fd)
{
	t_client *tmp = clients;
	t_client *to_delete = NULL;
	int id = get_id(fd);

	if (tmp && tmp->fd == fd)
	{
		to_delete = tmp;
		clients = tmp->next;
		close(to_delete->fd);
		free(to_delete);
	}
	else
	{
		while (tmp && tmp->next && tmp->next->fd != fd)
			tmp = tmp->next;
		to_delete = tmp->next;
		tmp->next = tmp->next->next;
		close(to_delete->fd);
		free(to_delete);
	}
	return (id);
}

void init_server(uint16_t port)
{
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_fatal();

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(port);

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		exit_fatal();
	if (listen(sockfd, 0) != 0)
		exit_fatal();
}

void init_server_values()
{
	FD_ZERO(&current_socket);
	FD_SET(sockfd, &current_socket);
	bzero(&msg, sizeof(msg));
}

void loop_server()
{
	while (1)
	{
		sock_write = sock_read = current_socket;
		if ((select(get_max_fd() + 1, &sock_read, &sock_write, NULL, NULL)) < 0)
			continue;
		if (FD_ISSET(sockfd, &sock_read))
			add_client();

		t_client *tmp = clients;

		while (tmp)
		{
			int id = tmp->id;
			int fd = tmp->fd;
			tmp = tmp->next;

			if (FD_ISSET(fd, &sock_read))
			{
				int max_size = 0;
				int size = 0;

				char msg_buffer[4096];
				char *message = NULL;

				bzero(&msg_buffer, sizeof(msg_buffer));

				while ((size = recv(fd, msg_buffer, 4095, 0)) > 0) {
					msg_buffer[size] = 0;
					max_size += size;
					if (!(message = str_join(message, msg_buffer)))
						exit_fatal();
					bzero(&msg_buffer, sizeof(msg_buffer));
				}

				if (max_size == 0)
				{
					int id_to_remove = id;

					if (id_to_remove != -1)
					{
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", id_to_remove);
						send_all(fd, msg);
						remove_client(fd);
					}
					FD_CLR(fd, &current_socket);
				}
				else if (max_size > 0)
				{
					char *tmp_message = NULL;
					char *message_to_send = NULL;

					while (extract_message(&message, &tmp_message)) {
						if (!(message_to_send = malloc(sizeof(char) * (strlen(tmp_message) + 42))))
							exit_fatal();
						sprintf(message_to_send, "client %d: %s", id, tmp_message);
						send_all(fd, message_to_send);
						free(tmp_message);
						free(message_to_send);
						tmp_message = NULL;
						message_to_send = NULL;
					}

					free(message);
					message = NULL;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		return (1);
	}
	init_server(atoi(argv[1]));
	init_server_values();
	loop_server();
	return (0);
}
