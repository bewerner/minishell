/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 22:21:58 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:03:59 by bwerner          ###   ########.fr       */
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

t_token	*token_last(t_token *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

void	token_add_back(t_token **lst, t_token *new)
{
	t_token	*ptr;

	ptr = token_last(*lst);
	if (*lst == NULL)
		*lst = new;
	else
		ptr->next = new;
}

t_token	*token_new(char *content)
{
	t_token	*newtoken;

	newtoken = (t_token *)ft_calloc(1, sizeof(t_token));
	if (newtoken == NULL)
		return (NULL);
	newtoken->content = content;
	return (newtoken);
}
