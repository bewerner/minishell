/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:36:45 by sgeiger           #+#    #+#             */
/*   Updated: 2024/06/11 15:38:29 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_valid_flag(char *str, bool uppercase)
{
	size_t	i;
	size_t	len;

	if (!str)
		return (false);
	len = ft_strlen(str);
	if (len < 2)
		return (false);
	if (str[0] != '-' || str[1] != 'n')
		return (false);
	if (uppercase && (str[2] == '\0' || ft_isspace(str[2])))
		return (true);
	else if (uppercase)
		return (false);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

void	exec_echo(t_leaf *leaf, t_token *token, t_minishell *ms, bool uppercase)
{
	bool	has_minus_n;
	size_t	i;

	has_minus_n = false;
	i = 1;
	while (i < leaf->size && is_valid_flag(token->content, uppercase))
	{
		has_minus_n = true;
		token = token->next;
		i++;
		if (uppercase)
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