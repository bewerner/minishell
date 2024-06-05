/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_export.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 21:26:53 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/05 02:45:49 by bwerner          ###   ########.fr       */
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
	while (env != tail)
	{
		if (!ft_strncmp(env->key, tail->key, ft_strlen(tail->key) +1))
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
	// if key already exists -> append
	// else remove '+' and then add_env
	add_env(content, ms);
}

void	export_key(char *content, t_minishell *ms)
{
	(void)content;
	(void)ms;
	// if key already exists -> return without adding anything
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
}
