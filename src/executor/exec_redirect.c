/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redirect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 22:16:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/14 17:36:35 by bwerner          ###   ########.fr       */
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
		if (ms->close_in_parent[0] != -1)
			close (ms->close_in_parent[0]);
		ms_error(leaf->head_token->next->content, NULL, EXIT_FAILURE, ms);
		ms->error = 0;
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
		if (leaf->left)
		{
			leaf->left->executed = true;
			leaf->left->child_pid = -1;
		}
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
	if (fd == -1)
	{
		if (ms->close_in_parent[0] != -1)
			close (ms->close_in_parent[0]);
		ms_error(leaf->head_token->next->content, NULL, EXIT_FAILURE, ms);
		ms->error = 0;
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(ms->fd_stdin_dup, STDIN_FILENO);
		if (leaf->left)
		{
			leaf->left->executed = true;
			leaf->left->child_pid = -1;
		}
	}
	else
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

t_env	*get_next_env(char *key, t_minishell *ms)
{
	t_env	*env;
	size_t	i;
	char	tmp;

	i = 1;
	while (key[i] && (ft_isalnum(key[i]) || key[i] == '_'))
		i++;
	tmp = key[i];
	key[i] = '\0';
	env = get_env(key, ms->head_env);
	key[i] = tmp;
	return (env);
}

size_t	get_next_key_len(char *key)
{
	size_t	i;

	i = 1;
	while (key[i] && (ft_isalnum(key[i]) || key[i] == '_'))
		i++;
	return (i);
}

void	put_expanded_content(char *content, int fd, t_minishell *ms)
{
	t_env	*env;
	size_t	i;

	i = 0;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1] == '?')
		{
			ft_putnbr_fd(ms->exit_code, fd);
			i++;
		}
		else if (content[i] == '$' && (ft_isalpha(content[i + 1])
				|| content[i + 1] == '_' || content[i + 1] == '?'))
		{
			env = get_next_env(content + i + 1, ms);
			if (env && env->value)
				ft_putstr_fd(env->value, fd);
			if (env)
				i += ft_strlen(env->key);
			else
				i += get_next_key_len(content + i + 1);
		}
		else
			ft_putchar_fd(content[i], fd);
		i++;
	}
}

void	exec_heredoc(t_input *heredoc, t_leaf *leaf, t_minishell *ms)
{
	int		p[2];

	pipe(p);
	dup2(p[0], STDIN_FILENO);
	if (ms->close_in_parent[0] != -1)
		close (ms->close_in_parent[0]);
	ms->close_in_parent[0] = p[0];
	while (heredoc)
	{
		if (heredoc->content)
		{
			if (!heredoc->literal)
				put_expanded_content(heredoc->content, p[1], ms);
			else
				ft_putstr_fd(heredoc->content, p[1]);
			ft_putchar_fd('\n', p[1]);
		}
		heredoc = heredoc->next;
	}
	close(p[1]);
	if (g_signal && leaf->left)
		leaf->left->executed = true;
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
	ms->exit_code = EXIT_FAILURE;
	if (leaf->left)
		leaf->left->executed = true;
	if (ms->close_in_parent[0] != -1)
		close (ms->close_in_parent[0]);
	return (true);
}

void	exec_redirect(t_leaf *leaf, t_minishell *ms)
{
	ms->exit_code = EXIT_SUCCESS;
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
