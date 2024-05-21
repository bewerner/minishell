/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 22:17:10 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/21 21:09:33 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	debug_print_tokens(t_token **head, size_t option)
{
	t_token	*token;

	if (!*head)
		return ;
	token = *head;
	if (option == 1)
		printf("\ntokens:             ");
	if (option == 2)
		printf("\nrearranged tokens:  ");
	while (token)
	{
		printf("(%d)(%d)%s, ", token->operator, token->type, token->content);
		token = token->next;
	}
	printf("\n                    ");
	token = *head;
	while (token)
	{
		printf("(%d)(%d)%s, ", token->operator, token->type, token->remove);
		token = token->next;
	}
	printf("\n");
}

void	debug_print_leafs(t_leaf **head)
{
	t_leaf	*leaf;
	t_token	*token;
	size_t	i;

	leaf = *head;
	if (leaf)
		printf("\nleafs:              ");
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
