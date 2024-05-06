/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rearrange_tokens.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 01:05:15 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/06 02:37:32 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_redirect_token(t_token *token)
{
	if (!token)
		return (false);
	if (token->type == TKN_IN
		|| token->type == TKN_OUT
		|| token->type == TKN_APPEND
		|| token->type == TKN_HEREDOC
		|| token->type == TKN_HERESTRING)
	{
		return (true);
	}
	return (false);
}

t_token	*get_previous_token(t_token **head, t_token *current)
{
	t_token	*prev;

	if (current == *head)
		return (NULL);
	prev = *head;
	while (prev->next != current)
		prev = prev->next;
	return (prev);
}

void	move_back_by_two(t_token *token, t_minishell *ms)
{
	t_token	*old_prev;
	t_token	*new_prev;
	t_token	*new_next;

	old_prev = get_previous_token(&ms->head_token, token);
	if (old_prev)
		old_prev->next = token->next;
	new_prev = token->next->next;
	new_next = token->next->next->next;
	new_prev->next = token;
	token->next = new_next;
}

void	rearrange_tokens(t_minishell *ms)
{
	t_token	*token;
	t_token	*prev;

	token = ms->head_token;
	while (token)
	{
		prev = get_previous_token(&ms->head_token, token);
		if (token->type == TKN_WORD && is_redirect_token(token->next))
		{
			if (token->next->next && !is_redirect_token(prev))
			{
				move_back_by_two(token, ms);
				token = ms->head_token;
			}
		}
		token = token->next;
	}
	if (ms->debug)
		debug_print_tokens(&ms->head_token);
}
