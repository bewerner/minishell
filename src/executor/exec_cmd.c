/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 23:14:57 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/17 22:22:33 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	init_leaf_content(t_leaf *leaf, t_minishell *ms)
{
	size_t	i;
	t_token	*token;

	if (ms->error)
		return ;
	i = 0;
	token = leaf->head_token;
	leaf->content = (char **)ft_calloc(leaf->size + 1, sizeof(char *));
	if (!leaf->content)
		ms_error("init_leaf_content", NULL, EXIT_FAILURE, ms);
	while (!ms->error && i < leaf->size)
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

char	*join_path(char *path, char *connector, char *cmd, t_minishell *ms)
{
	char	*rt;
	size_t	size;

	size = ft_strlen(path) + ft_strlen(connector) +  ft_strlen(cmd) + 1;
	rt = (char *)ft_calloc(size, sizeof(char));
	if (!rt)
	{
		ms_error("join_path: ft_calloc", NULL, EXIT_FAILURE, ms);
		return (NULL);
	}
	ft_strlcat(rt, path, size);
	ft_strlcat(rt, connector, size);
	ft_strlcat(rt, cmd, size);
	return (rt);
}

bool	is_directory(char *path)
{
	struct stat	tmp;

	stat(path, &tmp);
	if (S_ISDIR(tmp.st_mode))
		return (true);
	return (false);
}

char	**get_paths_from_env(t_minishell *ms)
{
	t_env	*env;
	char	**paths;

	env = get_env("PATH", ms->head_env);
	if (!env || !env->value || env->value[0] == '\0')
		return (NULL);
	paths = ft_split(env->value, ':');
	if (!paths)
		ms_error("get_paths_from_env: ft_split", NULL, EXIT_FAILURE, ms);
	return (paths);
}

void	set_env_path(t_leaf *leaf, char *cmd, char **paths, t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (paths[i])
	{
		leaf->path = join_path(paths[i], "/", cmd, ms);
		if (!leaf->path) // malloc failed
			break ;
		if (!access(leaf->path, F_OK) && !is_directory(leaf->path)) // match found
			break;
		free(leaf->path);
		leaf->path = NULL;
		i++;
	}
	if (!leaf->path && !ms->error) // no match found
		ms_error(cmd, "command not found", 127, ms);
	i = 0;
	while (paths[i]) // free paths
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

void	set_path(t_leaf *leaf, char *cmd, t_minishell *ms)
{
	char	**paths;

	if (is_path(cmd)) // cmd ist path. e.g.: ./minishell
	{
		leaf->path = ft_strdup(cmd);
		if (!leaf->path)
			ms_error("get_path: ft_strdup", NULL, EXIT_FAILURE, ms);
		else if (access(leaf->path, F_OK))
			ms_error(leaf->path, "No such file or directory", 127, ms);
		return ;
	}
	paths = get_paths_from_env(ms);
	if (ms->error)
		return ;
	if (!paths) // cmd is command. e.g.: grep, but PATH is not set
	{
		leaf->path = ft_strdup(cmd);
		if (!leaf->path)
			ms_error("get_path: ft_strdup", NULL, EXIT_FAILURE, ms);
		else if (access(leaf->path, F_OK))
			ms_error(leaf->path, "command not found", 127, ms);
		return ;
	}
	set_env_path(leaf, cmd, paths, ms); // cmd is command. e.g.: grep, and PATH is set
}

void	exec_cmd(t_leaf *leaf, char *cmd, t_minishell *ms)
{
	if (!ft_strncmp(cmd, ".", 2))
	{
		ms_error(".", "filename argument required", 2, ms);
		ft_putstr_fd(".: usage: . filename [arguments]\n", 2);
		return ;
	}
	set_path(leaf, cmd, ms);
	init_leaf_content(leaf, ms);
	if (!ms->error && leaf->path)
	{
		execve(leaf->path, leaf->content, ms->envp);
		if (is_directory(leaf->path))
			ms_error(leaf->path, "is a directory", 126, ms);
		else
			ms_error(cmd, NULL, 126, ms);
	}
}

void	exec_x(char *cmd, t_leaf *leaf, t_minishell *ms)
{
	ms->exit_code = EXIT_SUCCESS;
	if (ft_strncmp(cmd, "echo", 5) == 0)
		exec_echo(leaf, leaf->head_token->next);
	else if (ft_strncmp(cmd, "cd", 3) == 0)
		exec_cd(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "pwd", 4) == 0)
		exec_pwd(ms);
	else if (ft_strncmp(cmd, "export", 7) == 0)
		exec_export(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "unset", 6) == 0)
		exec_unset(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "env", 4) == 0)
		exec_env(ms);
	else if (ft_strncmp(cmd, "exit", 5) == 0)
		exec_exit(leaf, leaf->head_token->next, ms);
	else
		exec_cmd(leaf, cmd, ms);
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
		close_fd_parent_child(false, true, ms);
		exec_x(leaf->head_token->content, leaf, ms);
		if (leaf->fork)
			terminate(ms->exit_code, ms);
	}
	close_fd_parent_child(true, false, ms);
	restore_std_fd(ms);
}
