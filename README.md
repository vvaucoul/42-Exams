# Exam-Rank-04

## 3ème Exam du tronc commun de 42.
* __Difficulté:__ ⭐⭐⭐
* Le code "microshell.c" permet de valider cet exam.

### Asuces: 

* Ne leakez aucun file descripteur
* Pensez à bien free le programme
* Lors des exams, il n'y a aucune norme.

> * lsof -c microshell

# Exam-Rank-05

## 4ème Exam du tronc commun de 42.
* __Difficulté:__ ⭐⭐

* Le compilateur clang++ étant très restrictif, il est assez facile de valider cet exam.
* Ne perdez pas de temps, il faut au moins 1h30/2h pour valider cet exam !

# Exam-Rank-06

## 5ème Exam du tronc commun de 42.
* __Difficulté:__ ⭐⭐⭐
* Code: mini_serv.c

### Asuces: 

* Votre programme n'est pas censé fonctionner sans la fonction fcntl donnée dans le sujet. Si c'est le cas, vous aurez un fail au test 9
* N'oubliez pas d'enlever la fonction fcntl avant de 'grademe' sinon, -42
* L'utilisation d'une structure client est indispensable pour cet exam
* L'utilisation de variables globales est recommandé afin d'éviter de perdre du temps. 

> * Linux: netcat 0.0.0.0 [Port] 
> * Mac: nc 0.0.0.0 [Port] 
