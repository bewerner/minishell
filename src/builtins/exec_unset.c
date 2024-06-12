/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_unset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 23:28:25 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/12 14:33:35 by bwerner          ###   ########.fr       */
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
		if (!str[i])
			return (true);
	}
	ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	ms->exit_code = EXIT_FAILURE;
	return (false);
}

void	exec_unset(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	size_t	i;
	t_env	*env;

	ms->exit_code = EXIT_SUCCESS;
	i = 1;
	while (i < leaf->size && !ms->error)
	{
		if (is_valid(token->content, ms))
		{
			env = get_env(token->content, ms->head_env);
			if (env)
			{
				remove_env(&ms->head_env, env);
				update_envp(ms->head_env, ms);
			}
		}
		token = token->next;
		i++;
	}
}
