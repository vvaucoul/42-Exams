/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell_logic.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 19:08:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/07/01 19:28:45 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

###########################
#	Logique Multi Pipes	  #
###########################

- INPUT = 0
- OUTPUT = 1

[1] Premier Pipe :
	- Pipe fds
		- Dup2 fds[OUT] -> STDOUT
		- Execute Commande
	- Sauvegarder le fds[IN] dans le fd_prev


[2] Loop Pipe :
	- Tant que pipe - 1 (pour laisser le dernier au pipe last)
	- Pipe fds
		- Dup2 fd_prev -> STDIN
		- Dup2 fds[OUT] -> STDOUT
		- Execute Commande
	- Sauvegarder le fds[IN] dans le fd_prev

[3] Last Pipe :
	- Dup2 fd_prev -> STDIN_FILENO
	- Execute Commande

--------------------
--- Explications ---
--------------------

Le premier et le dernier pipe sont spéciaux,
le premier initialise la série et le dernier execute la commande en output

Pour le premier fork et le fork du pipe loop :
	- close [fds[INPUT]]
	- dup2 fds[OUT] -> STDOUT
	- Execute Commande

Pour le dernier fork :
	- dup2 fd_prev -> STDIN_FILENO
	(puisqu'il doit récupérer la suite de tous les pipes et l'envoyer vers l'entrée de la dernière commande)

Pour le premier et loop pipe :
	- Pipe(fds)
 	(le dernier n'a pas besoin de pipe)

Pour chaque :
	close respectivement les fds inutilisés
	pour le premier et loop :
		--- if ! fork
		- close [fds[input]]
		--- else fork
		- close [fds[output]]
	pour le dernier
		--- else fork
		- close [fds[input]]

###########################
#	  Logique PARSEUR	  #
###########################

- pour i = 1 à argc ++i
	- tant que argv[i] == ";"
		++i;
	- Si argv[i] != ; && argv[i] != |
		- Assigner actuel argv[i] dans le tableau args[current_arg].argd[j] et + 1 à NULL
		-++j
	- Sinon si argv[i] == |
		- Assigner acutel args pipe à 1
		- ++current_arg
	- Si i + 1 < argc
		- Si argv[j] != ; && argv[j + 1] == ;
			- Assigner acutel args pipe à 0
			- ++current_arg
			- j = 0

- NB Args = arg len (une simple boucle tant que args[i].args[0] != NULL alors ++i)

###########################
#	  	  Logique CD	  #
###########################

- Faire le CD en dehors du fork sinon ça fonctionne pas.
	- chdir [DIR]
		- 0 == SUCCESS
		- -1 == FAILURE
