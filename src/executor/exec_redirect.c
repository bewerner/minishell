/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:16:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/22 22:04:53 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_redirect_out(t_leaf *leaf, int options, t_minishell *ms)
{
	int	fd;


	leaf->left->read_pipe[0] = leaf->read_pipe[0];
	leaf->left->read_pipe[1] = leaf->read_pipe[1];
	fd = open(leaf->head_token->next->content, options, 0644);
	if (fd == -1)
	{
		ms_error(leaf->head_token->next->content, NULL, 1, ms);
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
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


	leaf->left->write_pipe[0] = leaf->write_pipe[0];
	leaf->left->write_pipe[1] = leaf->write_pipe[1];
	fd = open(leaf->head_token->next->content, O_RDONLY);
	if (fd == -1)
	{
		ms_error(leaf->head_token->next->content, NULL, 1, ms);
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
		leaf->left->executed = true;
	}
	else
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

void	exec_heredoc(t_leaf *leaf, t_minishell *ms)
{
	char	*line;
	char	*delimiter;
	int		p[2];

	(void)ms;
	set_signal(SIGINT, sigint_handler_heredoc);
	if (leaf->left)
	{
		leaf->left->write_pipe[0] = leaf->write_pipe[0];
		leaf->left->write_pipe[1] = leaf->write_pipe[1];
	}
	delimiter = leaf->head_token->next->content;
	pipe(p);
	line = readline("> ");
	while (line && ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1))
	{
		write(p[1], line, ft_strlen(line));
		write(p[1], "\n", 1);
		free(line);
		line = readline("> ");
	}
	if (!line && errno)
		ms_error("readline", NULL, 1, ms);
	free(line);
	if (leaf->left)
	{
		leaf->left->read_pipe[0] = p[0];
		leaf->left->read_pipe[1] = p[1];
	}
	if (g_signal && leaf->left)
		leaf->left->executed = true;
	set_signal(SIGINT, sigint_handler);
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
