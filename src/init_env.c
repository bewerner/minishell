/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 00:54:17 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/10 01:49:48 by bwerner          ###   ########.fr       */
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

void	init_env(char **envp, t_minishell *ms)
{
	t_env	*env;
	char	*content;
	size_t	i;

	i = 0;
	while (envp[i])
	{
		content = ft_strdup(envp[i]);
		env = env_new(content);
		if (!env || !content)
		{
			free(content);
			free(env);
			ms_error("init_env", NULL, 1, ms);
			cleanup(ms);
			exit (1);
		}
		env_add_back(&ms->head_env, env);
		i++;
	}
}
