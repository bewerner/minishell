/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 23:14:57 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/06 01:54:25 by bwerner          ###   ########.fr       */
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
	env_path = get_env("PATH", ms->head_env)->value;
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

void	exec_path(t_leaf *leaf, t_minishell *ms)
{
	char	*path;

	// close(ms->close_in_parent);
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
}

void	exec_cmd(char *cmd, t_leaf *leaf, t_minishell *ms)
{
		if (ft_strncasecmp(cmd, "echo", 5) == 0)
			exec_echo(leaf, leaf->head_token->next, ms);
		// else if (ft_strncasecmp(cmd, "cd", 3) == 0)
		// 	exec_cd(leaf, ms);
		else if (ft_strncasecmp(cmd, "pwd", 4) == 0)
			exec_pwd(ms);
		else if (ft_strncasecmp(cmd, "export", 7) == 0)
			exec_export(leaf, leaf->head_token->next, ms);
		else if (ft_strncasecmp(cmd, "unset", 6) == 0)
			exec_unset(leaf, leaf->head_token->next, ms);
		else if (ft_strncasecmp(cmd, "env", 4) == 0)
			exec_env(ms);
		else if (ft_strncasecmp(cmd, "exit", 5) == 0)
			exec_exit(leaf, leaf->head_token->next, ms);
		else
			exec_path(leaf, ms);
}

void	exec_word(t_leaf *leaf, t_minishell *ms)
{
	set_signal(SIGINT, sigint_handler_exec);
	set_signal(SIGQUIT, sigquit_handler_exec);
	if (leaf->fork)
		leaf->child_pid = fork();
	if (!leaf->child_pid || !leaf->fork)
	{
		set_signal(SIGQUIT, SIG_DFL);
		set_signal(SIGINT, SIG_DFL);
		if (ms->close_in_child != -1)
			close(ms->close_in_child);
		exec_cmd(leaf->head_token->content, leaf, ms);
		if (leaf->fork)
			terminate(ms->exit_code, ms);
	}
	if (ms->close_in_parent != -1)
		close(ms->close_in_parent);
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
}

// void	exec_path(t_leaf *leaf, t_minishell *ms)
// {
// 	char	*path;

// 	set_signal(SIGINT, sigint_handler_exec);
// 	set_signal(SIGQUIT, sigquit_handler_exec);
// 	leaf->child_pid = fork();
// 	if (leaf->child_pid == 0)
// 	{
// 		set_signal(SIGQUIT, SIG_DFL);
// 		set_signal(SIGINT, SIG_DFL);
// 		if (ms->close_in_child != -1)
// 			close(ms->close_in_child);
// 		close(ms->close_in_parent);
// 		path = get_path(leaf->head_token->content, ms);
// 		init_leaf_content(leaf);
// 		if (path && execve(path, leaf->content, ms->envp) == -1)
// 		{
// 			if (is_directory(path))
// 				ms_error(path, "is a directory", 126, ms);
// 			else
// 				ms_error(path, NULL, 126, ms);
// 		}
// 		if (errno == EACCES)
// 			ms->exit_code = 126;
// 		free(path);
// 		terminate(ms->exit_code, ms);	// find out exit code. also print error.
// 	}
// 	if (ms->close_in_parent != -1)
// 		close(ms->close_in_parent);
// 	dup2(ms->fd_stdin_dup, STDIN_FILENO);
// 	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
// }
