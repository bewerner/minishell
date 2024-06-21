/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 22:26:41 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:07:12 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_inputs(t_input **head)
{
	t_input	*ptr;
	t_input	*next;

	ptr = *head;
	while (ptr)
	{
		next = ptr->next;
		free(ptr->content);
		free(ptr);
		ptr = next;
	}
	*head = NULL;
}

t_input	*input_last(t_input *lst)
{
	if (lst == NULL)
		return (NULL);
	while (lst->next != NULL)
	{
		lst = lst->next;
	}
	return (lst);
}

void	input_add_back(t_input **lst, t_input *new)
{
	t_input	*ptr;

	ptr = input_last(*lst);
	if (*lst == NULL)
		*lst = new;
	else
		ptr->next = new;
}

t_input	*input_new(char *content)
{
	t_input	*newinput;

	newinput = (t_input *)ft_calloc(1, sizeof(t_input));
	if (newinput == NULL)
		return (NULL);
	newinput->content = content;
	return (newinput);
}
