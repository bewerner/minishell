/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:16:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 01:19:09 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_redirect_out(t_leaf *leaf, int options, t_minishell *ms)
{
	int	fd;

	fd = open(leaf->head_token->next->content, options, 0644);
	if (fd == -1)
	{
		close_fd_parent_child(true, false, ms);
		restore_std_fd(ms);
		leaf->exit_code = EXIT_FAILURE;
		ms_error(leaf->head_token->next->content, NULL, EXIT_FAILURE, ms);
		ms->error = 0;
		if (leaf->left)
			leaf->left->executed = true;
	}
	else
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}

void	exec_redirect_in(t_leaf *leaf, t_minishell *ms)
{
	int	fd;

	fd = open(leaf->head_token->next->content, O_RDONLY);
	if (fd == -1)
	{
		close_fd_parent_child(true, false, ms);
		restore_std_fd(ms);
		leaf->exit_code = EXIT_FAILURE;
		ms_error(leaf->head_token->next->content, NULL, EXIT_FAILURE, ms);
		ms->error = 0;
		if (leaf->left)
			leaf->left->executed = true;
	}
	else
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

bool	is_ambiguous(t_leaf *leaf, t_minishell *ms)
{
	if (leaf->size == 2)
		return (false);
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (leaf->size == 1)
		ft_putstr_fd(leaf->head_token->original_next_content, STDERR_FILENO);
	else
		ft_putstr_fd(leaf->head_token->next->original_content, STDERR_FILENO);
	ft_putendl_fd(": ambiguous redirect", STDERR_FILENO);
	leaf->exit_code = EXIT_FAILURE;
	if (leaf->left)
		leaf->left->executed = true;
	close_fd_parent_child(true, true, ms);
	restore_std_fd(ms);
	return (true);
}

void	exec_redirect(t_leaf *leaf, t_minishell *ms)
{
	leaf->exit_code = EXIT_SUCCESS;
	if (is_ambiguous(leaf, ms))
		return ;
	if (leaf->head_token->type == TKN_OUT)
		exec_redirect_out(leaf, O_WRONLY | O_CREAT | O_TRUNC, ms);
	else if (leaf->head_token->type == TKN_APPEND)
		exec_redirect_out(leaf, O_WRONLY | O_CREAT | O_APPEND, ms);
	else if (leaf->head_token->type == TKN_IN)
		exec_redirect_in(leaf, ms);
	else if (leaf->head_token->type == TKN_HEREDOC)
		exec_heredoc(leaf->head_token->head_heredoc, leaf, ms);
}
