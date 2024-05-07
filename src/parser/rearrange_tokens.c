/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rearrange_tokens.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 01:05:15 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/07 21:12:35 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

void	move_back_by_two(t_token **head_token, t_token *token)
{
	t_token	*old_prev;
	t_token	*old_next;
	t_token	*new_prev;
	t_token	*new_next;

	old_prev = get_previous_token(head_token, token);
	if (old_prev)
		old_prev->next = token->next;
	old_next = token->next;
	new_prev = token->next->next;
	new_next = token->next->next->next;
	new_prev->next = token;
	token->next = new_next;
	if (token == *head_token)
		*head_token = old_next;
}

void	rearrange_tokens(t_minishell *ms)
{
	t_token	*token;
	t_token	*prev;

	if (ms->error)
		return ;
	token = ms->head_token;
	while (token)
	{
		prev = get_previous_token(&ms->head_token, token);
		if (token->type == TKN_WORD
			&& token->next && token->next->type == OP_REDIRECT)
		{
			if (token->next->next && prev->operator != OP_REDIRECT)
			{
				move_back_by_two(&ms->head_token, token);
				token = ms->head_token;
				continue ;
			}
		}
		token = token->next;
	}
	if (ms->debug)
		debug_print_tokens(&ms->head_token);
}
