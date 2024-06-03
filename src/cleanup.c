/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 23:05:47 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/03 21:31:50 by bwerner          ###   ########.fr       */
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
	// close(3);
	// close(4);
	ms->close_in_child = -1;
	ms->close_in_parent = -1;
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
	free(ms->line);
	ms->line = NULL;
	free_inputs(&ms->head_input);
	free_leafs(&ms->head_leaf);
	free_tokens(&ms->head_token);
	if (g_signal && !ms->exit_code)
		ms->exit_code = 1;
	g_signal = 0;
	ms->error = false;
}

void	terminate(uint8_t exit_code, t_minishell *ms)
{
	ms->exit_code = exit_code;
	cleanup(ms);
	free_env(&ms->head_env);
	close(ms->fd_stdin_dup);
	close(ms->fd_stdout_dup);
	rl_clear_history();
	exit(ms->exit_code);
}
