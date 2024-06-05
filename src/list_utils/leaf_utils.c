/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leaf_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 22:27:54 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/05 22:32:59 by bwerner          ###   ########.fr       */
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
	{
		ptr->next = new;
		new->prev = ptr;
	}
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
