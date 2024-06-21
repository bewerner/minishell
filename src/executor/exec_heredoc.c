/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 01:19:15 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 01:19:38 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*get_next_env(char *key, t_minishell *ms)
{
	t_env	*env;
	size_t	i;
	char	tmp;

	i = 1;
	while (key[i] && (ft_isalnum(key[i]) || key[i] == '_'))
		i++;
	tmp = key[i];
	key[i] = '\0';
	env = get_env(key, ms->head_env);
	key[i] = tmp;
	return (env);
}

size_t	get_next_key_len(char *key)
{
	size_t	i;

	i = 1;
	while (key[i] && (ft_isalnum(key[i]) || key[i] == '_'))
		i++;
	return (i);
}

void	put_expanded_content(char *content, int fd, t_minishell *ms)
{
	t_env	*env;
	size_t	i;

	i = 0;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1] == '?')
		{
			ft_putnbr_fd(ms->exit_code, fd);
			i++;
		}
		else if (content[i] == '$'
			&& (ft_isalpha(content[i + 1]) || content[i + 1] == '_'))
		{
			env = get_next_env(content + i + 1, ms);
			if (env && env->value)
				ft_putstr_fd(env->value, fd);
			i += get_next_key_len(content + i + 1);
		}
		else
			ft_putchar_fd(content[i], fd);
		i++;
	}
}

void	exec_heredoc(t_input *heredoc, t_leaf *leaf, t_minishell *ms)
{
	int		p[2];

	pipe(p);
	dup2(p[0], STDIN_FILENO);
	ms->close_in_parent[2] = p[0];
	while (heredoc)
	{
		if (heredoc->content)
		{
			if (!heredoc->literal)
				put_expanded_content(heredoc->content, p[1], ms);
			else
				ft_putstr_fd(heredoc->content, p[1]);
			ft_putchar_fd('\n', p[1]);
		}
		heredoc = heredoc->next;
	}
	close(p[1]);
	if (g_signal && leaf->left)
		leaf->left->executed = true;
}
