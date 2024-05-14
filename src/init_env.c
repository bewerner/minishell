/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 00:54:17 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/14 00:24:31 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*env_last(t_env *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

void	env_add_back(t_env **lst, t_env *new)
{
	t_env	*ptr;

	ptr = env_last(*lst);
	if (*lst == NULL)
		*lst = new;
	else
	{
		ptr->next = new;
		new->prev = ptr;
	}
}

t_env	*env_new(char *content)
{
	t_env	*newenv;

	newenv = (t_env *)ft_calloc(1, sizeof(t_env));
	if (newenv == NULL)
		return (NULL);
	newenv->content = content;
	return (newenv);
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
}

// void	init_env(char **envp, t_minishell *ms)
// {
// 	t_env	*env;
// 	char	*content;
// 	size_t	i;

// 	i = 0;
// 	while (envp[i])
// 	{
// 		content = ft_strdup(envp[i]);
// 		env = env_new(content);
// 		if (env && content)
// 			env_add_back(&ms->head_env, env);
// 		if (!env || !content || !init_key_and_value(env))
// 		{
// 			if (env)
// 			{
// 				free(env->key);
// 				free(env->value);
// 			}
// 			free(content);
// 			free(env);
// 			ms_error("init_env", NULL, 1, ms);
// 			terminate(1, ms);
// 		}
// 		i++;
// 	}
// }
