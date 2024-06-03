/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:16:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/03 21:38:32 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_redirect_out(t_leaf *leaf, int options, t_minishell *ms)
{
	int	fd;

	fd = open(leaf->head_token->next->content, options, 0644);
	// if (ms->close_in_parent != -1)
	// 	close (ms->close_in_parent);
	if (fd == -1)
	{
		ms_error(leaf->head_token->next->content, NULL, 1, ms);
		ms->error = 0;
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
		leaf->left->executed = true;
		leaf->left->child_pid = -1;
	}
	else
	{
		// fprintf(stderr, "(>) dup2(%d, %d)\n", fd, STDOUT_FILENO);
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}

void	exec_redirect_in(t_leaf *leaf, t_minishell *ms)
{
	int	fd;

	fd = open(leaf->head_token->next->content, O_RDONLY);
	if (ms->close_in_parent != -1)
		close (ms->close_in_parent);
	if (fd == -1)
	{
		ms_error(leaf->head_token->next->content, NULL, 1, ms);
		ms->error = 0;
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
		leaf->left->executed = true;
		leaf->left->child_pid = -1;
	}
	else
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

void	exec_heredoc(t_leaf *leaf, t_minishell *ms)
{
	t_input	*heredoc;
	int		p[2];

	(void)ms;
	heredoc = leaf->head_token->head_heredoc;
	pipe(p);
	dup2(p[0], STDIN_FILENO);
	if (ms->close_in_parent != -1)
		close (ms->close_in_parent);
	ms->close_in_parent = p[0];
	while (heredoc)
	{
		if (heredoc->content)
		{
			ft_putstr_fd(heredoc->content, STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		heredoc = heredoc->next;
	}
	close(p[1]);
	if (g_signal && leaf->left)
		leaf->left->executed = true;
}

void	exec_redirect(t_leaf *leaf, t_minishell *ms)
{
	if (leaf->head_token->type == TKN_OUT)
		exec_redirect_out(leaf, O_WRONLY | O_CREAT | O_TRUNC, ms);
	else if (leaf->head_token->type == TKN_APPEND)
		exec_redirect_out(leaf, O_WRONLY | O_CREAT | O_APPEND, ms);
	else if (leaf->head_token->type == TKN_IN)
		exec_redirect_in(leaf, ms);
	else if (leaf->head_token->type == TKN_HEREDOC)
		exec_heredoc(leaf, ms);
}
