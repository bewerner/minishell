/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 00:54:17 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:16:30 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	export_key(char *key, t_minishell *ms)
{
	t_env	*env;

	if (get_env(key, ms->head_env))
		return ;
	env = env_new(NULL);
	if (env)
	{
		env_add_back(&ms->head_env, env);
		env->key = ft_strdup(key);
	}
	if (!env || !env->key)
	{
		free(env);
		ms_error("export_key", NULL, EXIT_FAILURE, ms);
	}
}

void	remove_duplicate_env(t_env *env, t_env *tail, t_minishell *ms)
{
	if (ms->error)
		return ;
	while (env != tail)
	{
		if (!ft_strncmp(env->key, tail->key, ft_strlen(tail->key) + 1))
		{
			remove_env(&ms->head_env, env);
			return ;
		}
		env = env->next;
	}
}

void	init_pwd(t_minishell *ms)
{
	char	*content;

	remove_env(&ms->head_env, get_env("OLDPWD", ms->head_env));
	export_key("OLDPWD", ms);
	if (ms->error)
		terminate(EXIT_FAILURE, ms);
	ms->cwd = getcwd(NULL, 0);
	if (!ms->cwd)
		ms_error("init_pwd: exec_pwd", NULL, EXIT_FAILURE, ms);
	else
		content = ft_strjoin("PWD=", ms->cwd);
	if (!ms->error && !content)
		ms_error("init_pwd: ft_strjoin", NULL, EXIT_FAILURE, ms);
	if (!ms->error)
	{
		add_env(content, ms);
		free(content);
		remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
	}
	if (ms->error)
		terminate(EXIT_FAILURE, ms);
}

void	init_env(char **envp, t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (envp[i])
	{
		add_env(envp[i], ms);
		if (ms->error)
			terminate(EXIT_FAILURE, ms);
		i++;
	}
	init_pwd(ms);
	sort_env(ms->head_env);
	update_envp(ms->head_env, ms);
	if (ms->error)
		terminate(EXIT_FAILURE, ms);
}
