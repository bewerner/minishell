/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 00:54:17 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/05 22:35:00 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_envp(t_env *env, t_minishell *ms)
{
	size_t	var_count;
	size_t	i;

	var_count = 0;
	while (env)
	{
		if (env->content)
			var_count++;
		env = env->next;
	}
	ms->envp = (char **)ft_calloc(var_count + 1, sizeof(char *));
	// failcheck
	env = ms->head_env;
	i = 0;
	while (env)
	{
		if (env->content)
		{
			ms->envp[i] = env->content;
			i++;
		}
		env = env->next;
	}
}

void	swap_env(t_env *a, t_env *b)
{
	t_env tmp;

	tmp.content = a->content;
	tmp.key = a->key;
	tmp.value = a->value;
	a->content = b->content;
	a->key = b->key;
	a->value = b->value;
	b->content = tmp.content;
	b->key = tmp.key;
	b->value = tmp.value;
}

void	sort_env(t_env	*head)
{
	t_env	*i;
	t_env	*j;

	if (!head || !head->next)
		return ;
	i = head;
	j = i->next;
	while (j)
	{
		if (ft_strncmp(i->key, j->key, ft_strlen(i->key) + 1) > 0)
		{
			swap_env(i, j);
			i = head;
			j = i->next;
			continue ;
		}
		i = i->next;
		j = j->next;
	}
}

bool	init_key_and_value(t_env *env)
{
	size_t	i;
	size_t	key_len;
	size_t	value_len;

	i = 0;
	key_len = 0;
	value_len = 0;
	while (env->content[i] && env->content[i] != '=')
		i++;
	key_len = i;
	env->key = (char *)ft_calloc(key_len + 1, sizeof(char));
	if (!env->key)
		return (false);
	ft_strlcpy(env->key, env->content, key_len + 1);
	if (env->content[i] == '\0')
		return (true);
	i++;
	while (env->content[i])
		i++;
	value_len = i - key_len - 1;
	env->value = (char *)ft_calloc(value_len + 1, sizeof(char));
	if (!env->value)
		return (false);
	ft_strlcpy(env->value, env->content + key_len + 1, value_len + 1);
	return (true);
}

bool	add_env(char *str, t_minishell *ms)
{
	char *content;
	t_env *env;

	content = ft_strdup(str);
	env = env_new(content);
	if (env && content)
		env_add_back(&ms->head_env, env);
	if (!env || !content || !init_key_and_value(env))
	{
		if (env)
		{
			free(env->key);
			free(env->value);
		}
		free(content);
		free(env);
		return (false);
	}
	return(true);
}

void	init_env(char **envp, t_minishell *ms)
{
	size_t	i;

	i = 0;
	while (envp[i])
	{
		if (!add_env(envp[i], ms))
		{
			ms_error("init_env", NULL, 1, ms);
			terminate(1, ms);
		}
		i++;
	}
	sort_env(ms->head_env);
	update_envp(ms->head_env, ms);
}
