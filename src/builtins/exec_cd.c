/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 20:16:52 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/16 21:46:51 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*get_target(char *key, t_minishell *ms)
{
	t_env	*env;

	env = get_env(key, ms->head_env);
	if (!env || !env->value || env->value[0] == '\0')
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(key, STDERR_FILENO);
		ft_putstr_fd(" not set\n", STDERR_FILENO);
		ms->exit_code = EXIT_FAILURE;
	}
	else
		return (env->value);
	return (NULL);
}

void	update_oldpwd(t_minishell *ms)
{
	t_env	*pwd;
	char	*content;

	pwd = get_env("PWD", ms->head_env);
	if (!pwd || !pwd->value || pwd->value[0] == '\0')
	{
		remove_env(&ms->head_env, get_env("OLDPWD", ms->head_env));
		export_key("OLDPWD", ms);
		return ;
	}
	content = ft_strjoin("OLDPWD=", pwd->value);
	if (!content)
		ms_error("update_pwd", NULL, EXIT_FAILURE, ms);
	add_env(content, ms);
	free(content);
	remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
}

void	update_pwd(char *cwd, t_minishell *ms)
{
	char	*content;

	content = NULL;
	if (ms->error)
		return ;
	ms->cwd = getcwd(NULL, 0);
	if (!ms->cwd && errno == ENOENT)
		ft_putendl_fd("minishell: cd: error retrieving current directory: \
getcwd: cannot access parent directories: \
No such file or directory", STDERR_FILENO);
	else if (!ms->cwd)
		ms_error("update_pwd: getcwd", NULL, EXIT_FAILURE, ms);
	if (!ms->error && !ms->cwd)
		ms->cwd = ft_strjoin(cwd, "/..");
	free(cwd);
	errno = 0;
	if (!ms->error && !ms->cwd)
		ms_error("update_pwd: ft_strjoin", NULL, EXIT_FAILURE, ms);
	if (!ms->error)
		content = ft_strjoin("PWD=", ms->cwd);
	if (!ms->error && !content)
		ms_error("update_pwd: ft_strjoin", NULL, EXIT_FAILURE, ms);
	if (!ms->error)
		add_env(content, ms);
	free(content);
	remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
}

void	exec_cd(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	char	*target;

	if (leaf->size == 1 || !ft_strncmp(token->content, "--", 3))
		target = get_target("HOME", ms);
	else if (!ft_strncmp(token->content, "-", 2))
		target = get_target("OLDPWD", ms);
	else
		target = token->content;
	if (target && chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ms_error(target, NULL, EXIT_FAILURE, ms);
		ms->error = false;
	}
	else if (target)
	{
		if (leaf->size > 1 && !ft_strncmp(token->content, "-", 2))
			ft_putendl_fd(target, STDOUT_FILENO);
		update_oldpwd(ms);
		update_pwd(ms->cwd, ms);
		sort_env(ms->head_env);
		update_envp(ms->head_env, ms);
	}
	if (leaf->fork)
		terminate(ms->exit_code, ms);
}

// void	exec_cd(t_leaf *leaf, t_token *token, t_minishell *ms)
// {
// 	char	*target;

// 	ms->exit_code = EXIT_SUCCESS;
// 	if (leaf->size == 1 || !ft_strncmp(token->content, "--", 3))
// 		target = get_target("HOME", ms);
// 	else if (!ft_strncmp(token->content, "-", 2))
// 		target = get_target("OLDPWD", ms);
// 	else
// 		target = token->content;
// 	if (target && chdir(target) == -1)
// 	{
// 		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
// 		perror(target);
// 		errno = 0;
// 		ms->exit_code = EXIT_FAILURE;
// 	}
// 	else if (target)
// 	{
// 		if (leaf->size > 1 && !ft_strncmp(token->content, "-", 2))
// 			ft_putendl_fd(target, STDOUT_FILENO);
// 		update_oldpwd(ms);
// 		update_pwd(ms->cwd, ms);
// 		sort_env(ms->head_env);
// 		update_envp(ms->head_env, ms);
// 	}
// 	if (leaf->fork)
// 		terminate(ms->exit_code, ms);
// 	return ;
// }
