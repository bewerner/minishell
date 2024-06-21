/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 23:57:00 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 00:00:57 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_path(char *str)
{
	if (ft_strchr(str, '/'))
		return (true);
	return (false);
}

char	*join_path(char *path, char *connector, char *cmd, t_minishell *ms)
{
	char	*rt;
	size_t	size;

	size = ft_strlen(path) + ft_strlen(connector) + ft_strlen(cmd) + 1;
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
