/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 20:16:52 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/11 19:24:13 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_target(char *key, t_minishell *ms)
{
	t_env	*env;

	env = get_env(key, ms->head_env);
	if (!env || env->value[0] == '\0')
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(key, STDERR_FILENO);
		ft_putstr_fd(" not set\n", STDERR_FILENO);
		ms->exit_code = 1;
	}
	else
		return (env->value);
	return (NULL);
}

void	update_pwd(t_minishell *ms)
{
	t_env	*pwd;
	char	*content;
	char	cwd[PATH_MAX];

	pwd = get_env("PWD", ms->head_env);
	if (!pwd || !pwd->value)
		content = ft_strdup("OLDPWD=");
	else
		content = ft_strjoin("OLDPWD=", pwd->value);
	if (!content)
		ms_error("update_pwd", NULL, 1, ms);
	add_env(content, ms);
	free(content);
	remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
	if (!ms->error && getcwd(cwd, sizeof(cwd)) == NULL)
		ms_error("exec_pwd", NULL, 1, ms);
	if (!ms->error)
		content = ft_strjoin("PWD=", cwd);
	if (!ms->error && !content)
		ms_error("update_pwd", NULL, 1, ms);
	add_env(content, ms);
	free(content);
	remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
	sort_env(ms->head_env);
}

void	exec_cd(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	char	*target;

	ms->exit_code = 0;
	if (leaf->size == 1 || !ft_strncmp(token->content, "--", 3))
		target = get_target("HOME", ms);
	else if (!ft_strncmp(token->content, "-", 2))
		target = get_target("OLDPWD", ms);
	else
		target = token->content;
	if (target && chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		perror(target);
		errno = 0;
		ms->exit_code = 1;
	}
	else if (target)
	{
		if (!ft_strncmp(token->content, "-", 2))
			ft_putendl_fd(target, STDOUT_FILENO);
		update_pwd(ms);
	}
	if (leaf->fork)
		terminate(ms->exit_code, ms);
	return ;
}
