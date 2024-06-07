/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_export.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 21:26:53 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/06 23:48:39 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static bool	is_valid(char *str, t_minishell *ms)
{
	size_t	i;

	i = 1;
	if (ft_isalpha(str[0]) || str[0] == '_')
	{
		while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
			i++;
		if (!str[i]|| str[i] == '=' || (str[i] == '+' && str[i + 1] == '='))
			return (true);
	}
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	ms->exit_code = 1;
	return (false);
}

void	print_export(t_env *env)
{
	while (env)
	{
		if (env->value)
			printf("declare -x %s=\"%s\"\n", env->key, env->value);
		else
			printf("declare -x %s\n", env->key);
		env = env->next;
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

void	export_key_value(char *content, t_minishell *ms)
{
	add_env(content, ms);
	remove_duplicate_env(ms->head_env, env_last(ms->head_env), ms);
}

void	export_append_value(char *content, t_minishell *ms)
{
	char	*key;
	char	*value;
	t_env	*env;
	char	*new_content;

	key = content;
	*ft_strchr(key, '+') = '\0';
	value = content + ft_strlen(key) + 2;
	env = get_env(key, ms->head_env);
	if (env)
	{
		new_content = ft_strjoin(env->content, value);
		if (!new_content)
		{
			ms_error("export_append_value", NULL, 1, ms);
			return ;
		}
		add_env(new_content, ms);
		free (new_content);
		remove_env(&ms->head_env, env);
		return ;
	}
	ft_memmove(content + ft_strlen(key), value - 1, ft_strlen(value) + 2);
	add_env(content, ms);
}

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
		ms_error("export_key", NULL, 1, ms);
	}
}

void	exec_export(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	size_t	i;

	ms->exit_code = 0;
	if (leaf->size == 1)
		print_export(ms->head_env);
	i = 1;
	while (i < leaf->size && !ms->error)
	{
		if (is_valid(token->content, ms))
		{
			if (!ft_strchr(token->content, '='))
				export_key(token->content, ms);
			else if (*(ft_strchr(token->content, '=') - 1) == '+')
				export_append_value(token->content, ms);
			else
				export_key_value(token->content, ms);
		}
		token = token->next;
		i++;
	}
	if (leaf->size > 1 && !ms->error)
	{
		sort_env(ms->head_env);
		update_envp(ms->head_env, ms);
	}
}
