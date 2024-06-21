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

void	set_env_path(t_leaf *leaf, char *cmd, char **paths, t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (paths[i])
	{
		leaf->path = join_path(paths[i], "/", cmd, ms);
		if (!leaf->path)
			break ;
		if (!access(leaf->path, F_OK) && !is_directory(leaf->path))
			break ;
		free(leaf->path);
		leaf->path = NULL;
		i++;
	}
	if (!leaf->path && !ms->error)
		ms_error(cmd, "command not found", 127, ms);
	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
	free(paths);
}

void	set_path(t_leaf *leaf, char *cmd, t_minishell *ms)
{
	char	**paths;

	if (is_path(cmd))
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
	if (!paths)
	{
		leaf->path = ft_strdup(cmd);
		if (!leaf->path)
			ms_error("get_path: ft_strdup", NULL, EXIT_FAILURE, ms);
		else if (access(leaf->path, F_OK))
			ms_error(leaf->path, "No such file or directory", 127, ms);
		return ;
	}
	set_env_path(leaf, cmd, paths, ms);
}

void	exec_cmd(t_leaf *leaf, char *cmd, t_minishell *ms)
{
	if (!ft_strncmp(cmd, ".", 2))
	{
		ms_error(".", "filename argument required", 2, ms);
		ft_putstr_fd(".: usage: . filename [arguments]\n", STDERR_FILENO);
		return ;
	}
	set_path(leaf, cmd, ms);
	init_leaf_content(leaf, ms);
	if (!ms->error && leaf->path)
	{
		if (is_directory(leaf->path))
			ms_error(leaf->path, "is a directory", 126, ms);
		if (!ms->error && access(leaf->path, X_OK))
			ms_error(cmd, NULL, 126, ms);
		if (!ms->error)
			execve(leaf->path, leaf->content, ms->envp);
		if (!ms->error)
			ms_error(cmd, NULL, 126, ms);
	}
}
