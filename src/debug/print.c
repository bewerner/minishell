/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 22:17:10 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/17 23:46:44 by bwerner          ###   ########.fr       */
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
		printf("\033[30m(%d)(%d)\033[0m%s  ", token->operator, token->type, token->content);
		token = token->next;
	}
	printf("\n                    ");
	token = *head;
	while (token)
	{
		if (token->remove)
		{
			printf("      ");
			for (size_t i = 0; token->remove[i]; i++)
			{
				if (token->remove[i] == '1')
					printf("x");
				else
					printf(" ");
			}
			printf("  ");
		}
		else
		{
			for (size_t i = 0; i < ft_strlen(token->content) + 8; i++)
				printf(" ");
		}
		// else
		// 	printf("      \033[30m%s  \033[0m", token->content);
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
		printf("\033[30m(%d)\033[0m", leaf->operator);
		printf("\033[30m(%d)\033[0m", leaf->type);
		while (i < leaf->size && token)
		{
			printf("%s   ", token->content);
			token = token->next;
			i++;
		}
		leaf = leaf->next;
	}
	printf("\n");
}
