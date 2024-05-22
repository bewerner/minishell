/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_word.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 23:14:57 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/22 22:13:32 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_leaf_content(t_leaf *leaf)
{
	size_t	i;
	t_token	*token;

	i = 0;
	token = leaf->head_token;
	leaf->content = (char **)ft_calloc(leaf->size + 1, sizeof(char *));
	while (i < leaf->size)
	{
		leaf->content[i] = token->content;
		token = token->next;
		i++;
	}
}

bool	is_path(char *str)
{
	// if (str[0] == '.' || str[0] == '/')
	// 	return (true);
	if (ft_strchr(str, '/'))
		return (true);
	return (false);
}

char	*join_path(char **env_path, const char *slash, char *cmd)
{
	size_t	path_len;
	size_t	i;
	char	*path;

	i = 0;
	while ((*env_path)[0] == ':')
		(*env_path)++;
	while ((*env_path)[i] && (*env_path)[i] != ':')
		i++;
	path_len = i + ft_strlen(slash) + ft_strlen(cmd) + 1;
	path = (char *)ft_calloc(path_len, sizeof(char));
	ft_memmove(path, *env_path, i);
	ft_memmove(path + i, slash, ft_strlen(slash));
	ft_memmove(path + i + ft_strlen(slash), cmd, ft_strlen(cmd));
	*env_path += i;
	return (path);
}

t_env	*get_env(t_env *env, char *key)
{
	while (env)
	{
		if (!ft_strncmp(env->content, key, ft_strlen(key)))
			if (env->content[ft_strlen(key)] == '=')
				return (env);
		env = env->next;
	}
	return (env);
}

char	*get_path(char *cmd, t_minishell *ms)
{
	char	*env_path;
	char	*path;

	if (is_path(cmd))
	{
		path = strdup(cmd);
		if (!path)
		{
			ms_error("executor", NULL, 1, ms);
			cleanup(ms);
			exit (1);
		}
		return (path);
	}
	env_path = get_env(ms->head_env, "PATH")->content + 5;
	while (env_path[0])
	{
		path = join_path(&env_path, "/", cmd);
		// printf("path is: %s\n", path);
		if (!access(path, F_OK))
			return (path);
		free(path);
	}
	return(NULL);
}

void	exec_word(t_leaf *leaf, t_minishell *ms)
{
	char	*path;

	set_signal(SIGINT, sigint_handler_exec);
	set_signal(SIGQUIT, sigquit_handler_exec);
	leaf->child_pid = fork();
	if (leaf->child_pid == 0)
	{
		set_signal(SIGQUIT, SIG_DFL);
		set_signal(SIGINT, SIG_DFL);
		if (leaf->write_pipe[0])
		{
			dup2(leaf->write_pipe[1], STDOUT_FILENO);
			close(leaf->write_pipe[0]);
		}
		if (leaf->read_pipe[0])
		{
			dup2(leaf->read_pipe[0], STDIN_FILENO);
			close(leaf->read_pipe[1]);
		}
		path = get_path(leaf->head_token->content, ms);
		if (!path)
			ms_error(leaf->head_token->content, "command not found", 127, ms);
		// printf("path is: %s\n", path);
		init_leaf_content(leaf);
		if (path && execve(path, leaf->content, ms->envp) == -1)
			ms_error(path, NULL, 1, ms);
		free(path);
		if (leaf->write_pipe[0])
			close(leaf->write_pipe[1]);
		if (leaf->read_pipe[0])
			close(leaf->read_pipe[0]);
		terminate(ms->exit_code, ms);	// find out exit code. also print error.
	}
	if (leaf->read_pipe[0])
	{
		// printf("closing %d and %d\n", leaf->read_pipe[0], leaf->read_pipe[1]);
		close(leaf->read_pipe[0]);
		close(leaf->read_pipe[1]);
	}
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
}
