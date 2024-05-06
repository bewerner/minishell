/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_leafs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 23:13:18 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/06 02:57:06 by bwerner          ###   ########.fr       */
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

	newleaf = (t_leaf *)malloc(sizeof(t_leaf));
	if (newleaf == NULL)
		return (NULL);
	ft_bzero(newleaf, sizeof(t_leaf));
	newleaf->head = head;
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
		token = leaf->head;
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

void	init_leafs(t_minishell *ms)
{
	t_leaf	*leaf;
	t_token	*token;

	token = ms->head_token;
	while (token)
	{
		leaf = leaf_new(token);
		leaf->size = get_leaf_size(&token);
		// leaf->size = 1;
		leaf_add_back(&ms->head_leaf, leaf);
		// token = token->next; // get leaf size will handle this
	}
	if (ms->debug)
		debug_print_leafs(&ms->head_leaf);
}
