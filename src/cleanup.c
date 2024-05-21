/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 23:05:47 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/21 21:06:10 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(t_token **head)
{
	t_token	*ptr;
	t_token	*next;

	ptr = *head;
	while (ptr)
	{
		next = ptr->next;
		free(ptr->content);
		free(ptr->remove);
		free(ptr->original_content);
		free(ptr);
		ptr = next;
	}
	*head = NULL;
}

void	free_leafs(t_leaf **head)
{
	t_leaf	*ptr;
	t_leaf	*next;

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

void	free_env(t_env **head)
{
	t_env	*ptr;
	t_env	*next;

	ptr = *head;
	while (ptr)
	{
		next = ptr->next;
		free(ptr->content);
		free(ptr->key);
		free(ptr->value);
		free(ptr);
		ptr = next;
	}
	*head = NULL;
}

void	cleanup(t_minishell *ms)
{
	free(ms->line);
	free_leafs(&ms->head_leaf);
	free_tokens(&ms->head_token);
}

void	terminate(uint8_t exit_code, t_minishell *ms)
{
	cleanup(ms);
	free_env(&ms->head_env);
	close(ms->fd_stdin_dup);
	close(ms->fd_stdout_dup);
	exit(exit_code);
}
