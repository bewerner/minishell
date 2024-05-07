/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_leafs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 23:13:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/07 21:12:21 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_leaf	*leaf_last(t_leaf *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

void	leaf_add_back(t_leaf **lst, t_leaf *new)
{
	t_leaf	*ptr;

	ptr = leaf_last(*lst);
	if (*lst == NULL)
		*lst = new;
	else
		ptr->next = new;
}

t_leaf	*leaf_new(t_token *head)
{
	t_leaf	*newleaf;

	newleaf = (t_leaf *)ft_calloc(1, sizeof(t_leaf));
	if (newleaf == NULL)
		return (NULL);
	newleaf->head_token = head;
	return (newleaf);
}

void	debug_print_leafs(t_leaf **head)
{
	t_leaf	*leaf;
	t_token	*token;
	size_t	i;

	leaf = *head;
	while (leaf)
	{
		i = 0;
		token = leaf->head_token;
		printf("(%d)", leaf->operator);
		printf("(%d)", leaf->type);
		while (i < leaf->size && token)
		{
			printf("%s ", token->content);
			token = token->next;
			i++;
		}
		printf(", ");
		leaf = leaf->next;
	}
	printf("\n");
}

enum e_leaf_type	get_leaf_type(t_token *head_token)
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
	else if (head_token->type == TKN_HERESTRING)
		return (LEAF_HERESTRING);
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
			ms_error("parser", NULL, 1, ms);
			break ;
		}
		leaf->type = get_leaf_type(leaf->head_token);
		leaf->operator = leaf->head_token->operator;
		leaf->size = get_leaf_size(&token);
		leaf_add_back(&ms->head_leaf, leaf);
		token = token->next;
	}
	if (ms->debug)
		debug_print_leafs(&ms->head_leaf);
}
