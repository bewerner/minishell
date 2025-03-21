/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_leafs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 23:13:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:25:09 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_leaf_type	get_leaf_type(t_token *head_token)
{
	if (head_token->type == TKN_WORD)
		return (LEAF_WORD);
	else if (head_token->type == TKN_PIPE)
		return (LEAF_PIPE);
	else if (head_token->type == TKN_OR)
		return (LEAF_OR);
	else if (head_token->type == TKN_AND)
		return (LEAF_AND);
	else if (head_token->type == TKN_IN)
		return (LEAF_IN);
	else if (head_token->type == TKN_OUT)
		return (LEAF_OUT);
	else if (head_token->type == TKN_APPEND)
		return (LEAF_APPEND);
	else if (head_token->type == TKN_HEREDOC)
		return (LEAF_HEREDOC);
	return (LEAF_WORD);
}

size_t	get_leaf_size(t_token **token)
{
	size_t	size;

	size = 1;
	if ((*token)->type == TKN_WORD)
	{
		while ((*token)->next && (*token)->next->type == TKN_WORD)
		{
			*token = (*token)->next;
			size++;
		}
	}
	else if ((*token)->operator == OP_REDIRECT)
	{
		if ((*token)->next && (*token)->next->type == TKN_WORD)
		{
			*token = (*token)->next;
			size++;
		}
	}
	return (size);
}

void	init_leafs(t_minishell *ms)
{
	t_leaf	*leaf;
	t_token	*token;

	if (ms->error)
		return ;
	token = ms->head_token;
	while (token)
	{
		leaf = leaf_new(token);
		if (!leaf)
		{
			ms_error("parser", NULL, EXIT_FAILURE, ms);
			break ;
		}
		leaf->type = get_leaf_type(leaf->head_token);
		leaf->operator = leaf->head_token->operator;
		leaf->size = get_leaf_size(&token);
		leaf->exit_code = -1;
		leaf_add_back(&ms->head_leaf, leaf);
		token = token->next;
	}
}
