/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_word.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 23:14:57 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/03 21:32:34 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// bool	token_content_is_empty(char *content)
// {
// 	size_t	i;

// 	i = 0;
// 	if (!content)
// 		return (true);
// 	while (content[i])
// 	{
// 		if (!ft_isspace(content[i]))
// 			return (false);
// 		i++;
// 	}
// 	return (true);
// }

// void	init_leaf_content(t_leaf *leaf)
// {
// 	size_t	i;
// 	size_t	j;
// 	t_token	*token;

// 	i = 0;
// 	j = 0;
// 	token = leaf->head_token;
// 	leaf->content = (char **)ft_calloc(leaf->size + 1, sizeof(char *));
// 	while (i < leaf->size)
// 	{
// 		if (!token_content_is_empty(token->content))
// 		{
// 			leaf->content[j] = token->content;
// 			j++;
// 		}
// 		token = token->next;
// 		i++;
// 	}
// }

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
		if (access(cmd, F_OK))
		{
			ms_error(cmd, "No such file or directory", 127, ms);
			return (NULL);
		}
		path = strdup(cmd);
		if (!path)
		{
			ms_error("executor", NULL, 1, ms);
			terminate(1, ms);
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
	ms_error(cmd, "command not found", 127, ms);
	return(NULL);
}

bool	is_directory(char *path)
{
	struct stat	tmp;

	stat(path, &tmp);
	if (S_ISDIR(tmp.st_mode))
		return (true);
	return (false);
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
		if (ms->close_in_child != -1)
			close(ms->close_in_child);
		close(ms->close_in_parent);
		path = get_path(leaf->head_token->content, ms);
		init_leaf_content(leaf);
		if (path && execve(path, leaf->content, ms->envp) == -1)
		{
			if (is_directory(path))
				ms_error(path, "is a directory", 126, ms);
			else
				ms_error(path, NULL, 126, ms);
		}
		if (errno == EACCES)
			ms->exit_code = 126;
		free(path);
		terminate(ms->exit_code, ms);	// find out exit code. also print error.
	}
	if (ms->close_in_parent != -1)
		close(ms->close_in_parent);
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
}
