/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 23:05:47 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 02:53:51 by bwerner          ###   ########.fr       */
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
		free(ptr->original_next_content);
		free_inputs(&ptr->head_heredoc);
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
		free(ptr->path);
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
	ms->in_pipeline = false;
	close_fd_parent_child(true, true, ms);
	restore_std_fd(ms);
	free(ms->line);
	ms->line = NULL;
	free_inputs(&ms->head_input);
	free_leafs(&ms->head_leaf);
	free_tokens(&ms->head_token);
	if (g_signal && !ms->exit_code)
		ms->exit_code = EXIT_FAILURE;
	g_signal = 0;
	ms->syntax_error = SYN_NONE;
	ms->syntax_error_input = NULL;
	ms->error = false;
}

void	terminate(int64_t exit_code, t_minishell *ms)
{
	ms->exit_code = exit_code;
	cleanup(ms);
	free(ms->cwd);
	free(ms->envp);
	free_env(&ms->head_env);
	close(ms->fd_stdin_dup);
	close(ms->fd_stdout_dup);
	rl_clear_history();
	exit(ms->exit_code);
}
