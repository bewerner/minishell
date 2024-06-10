/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:36:45 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/10 05:29:51 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_valid_flag(t_token *token)
{
	size_t	i;
	size_t	len;

	if (!token->content)
		return (false);
	len = ft_strlen(token->content);
	if (len < 2)
		return (false);
	if (token->content[0] != '-' || token->content[1] != 'n')
		return (false);
	i = 1;
	while (token->content[i])
	{
		if (token->content[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

void	exec_echo(t_leaf *leaf, t_token *token, t_minishell *ms)
{
	bool	has_minus_n;
	size_t	i;

	has_minus_n = false;
	i = 1;
	while (i < leaf->size && is_valid_flag(token))
	{
		has_minus_n = true;
		token = token->next;
		i++;
		if (!ms->interactive)
			break ;
	}
	while (i < leaf->size)
	{
		if (token->content)
			ft_putstr_fd(token->content, STDOUT_FILENO);
		if (i + 1 < leaf->size && token->next->content)
			ft_putchar_fd(' ', STDOUT_FILENO);
		token = token->next;
		i++;
	}
	if (!has_minus_n)
		ft_putchar_fd('\n', STDOUT_FILENO);
	ms->exit_code = 0;
}

// while (token)
// {
// 	if (ms->close_in_child != -1)
// 	{
// 		ft_putstr_fd(token->content, ms->close_in_child);
// 		ft_putchar_fd(' ', ms->close_in_child);
// 	}
// 	else
// 	{
// 		ft_putstr_fd(token->content, 1);
// 		ft_putchar_fd(' ', 1);
// 	}
// 	token = token->next;
// }
// if (!has_minus_n)
// {
// 	if (ms->close_in_child != -1)
// 		ft_putchar_fd('\n', ms->close_in_child);
// 	else
// 		ft_putchar_fd('\n', 1);
// }