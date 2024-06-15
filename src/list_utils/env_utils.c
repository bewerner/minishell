/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 22:15:11 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/15 21:50:06 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*get_env(char *key, t_env *env)
{
	while (env)
	{
		if (!ft_strncmp(key, env->key, ft_strlen(key) + 1))
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	remove_env(t_env **head, t_env *env)
{
	if (!env)
		return ;
	if (env == *head)
	{
		*head = env->next;
		(*head)->prev = NULL;
	}
	else if (!env->next)
		env->prev->next = NULL;
	else
	{
		env->prev->next = env->next;
		env->next->prev = env->prev;
	}
	free(env->content);
	free(env->key);
	free(env->value);
	free(env);
}

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
