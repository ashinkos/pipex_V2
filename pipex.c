/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaouni <aaouni@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/28 01:08:51 by aaouni            #+#    #+#             */
/*   Updated: 2022/07/30 16:23:33 by aaouni           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int len_2d(char **str)
{
	int i;

	i = -1;
	while (str[++i])
		;
	return (i);
}
void	free_2d(char ** str)
{
	int i;
	
	i=0;
	while (str[i])
	{	
		free(str[i]);
		i++;
	}
	free(str);
}

char	**get_path_env(char **env)
{
	char	**path;
	char	**path_slash;
	char	*path_str;
	int		i;
	int		j;
	
	j = 0;
	i = 0;
	while(env[i])
	{
		if(ft_strncmp(env[i], "PATH", 4) == 0)
		{
			path_str = env[i] + 5;
			path = ft_split(path_str, ':');
			path_slash = malloc (sizeof(char *) * (len_2d(path)+ 1));
			if (!path_slash)
				return (NULL);
			while(path[j])
			{
				path_slash[j] = ft_strjoin(path[j], "/");
				j++;	
			}
			free_2d(path);
			path_slash[j] = NULL;
			return (path_slash);
		}
		i++;
	}
	return (NULL);
}

char	**get_path_cmd(char *str, char **path)
{
	char	**cmd;
	char	*path_cmd;
	
	cmd = ft_split(str, ' ');
	if(access(cmd[0], X_OK) == 0)
		return(cmd);
	while(*path)
	{
		path_cmd = ft_strjoin(*path, cmd[0]);
		if(access(path_cmd, X_OK) == 0)
		{
			free (cmd [0]);
			cmd[0] = ft_strdup(path_cmd);
			free(path_cmd);
			return (cmd);
			
		}
		free(path_cmd);
		(*path)++;	
	}
	return (NULL);
}


int main(int argc, char **argv, char **env)
{
	int		fd[2];
	int		pid1;
	int		pid2;
	char	**path;
	char	**cmd1;
	char	**cmd2;
	int		infile;
	int		outfile;
	int		i;
	
	
	i = 0;
	cmd1 = NULL;
	path = NULL;
	
	if (argc == 5 && argv)
	{
		path = get_path_env(env);
		if (access(argv[1], F_OK) < 0)
			perror("pipex");
		cmd1 = get_path_cmd(argv[2], path);
		infile = open(argv[1], O_RDONLY);
		outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (pipe(fd) == -1)
			exit(1);
		pid1 = fork();
		if (pid1 < 0)
			exit(1);
		if (pid1 == 0)
		{
			close(fd[0]);
			dup2(infile, STDIN_FILENO);
			dup2(fd[1], STDOUT_FILENO);
			close(infile);
			close(fd[1]);
			if(execve(cmd1[0], cmd1, env) < 0)
				perror("pipex");
			
		}
		pid2 = fork();
		if (pid2 < 0)
			exit(1);
		if (pid2 == 0)
		{
			cmd2 = get_path_cmd(argv[3], path);
			close(fd[1]);
			dup2(outfile, STDOUT_FILENO);
			dup2(fd[0], STDIN_FILENO);
			close(outfile);
			close(fd[0]);
			if(execve(cmd2[0], cmd2, env) < 0)	
				perror("pipex");
		}
		close(fd[1]);
		close(fd[0]);
		while (waitpid(-1, NULL, 0) > 0)
			;		
	}
	// i = 0;
	// if(cmd1)
	// {
	// 	while(cmd1[i] != NULL)
	// 	{
			
	// 		printf("%s\n",cmd1[i]);
	// 		i++;
	// 	}
	// }
	system("leaks pipex");
	return 0;
}
