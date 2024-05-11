/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_executor.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 20:23:20 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/11 02:59:08 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_logical(t_leaf *leaf, t_minishell *ms)
{
	if (ms->exit_code == 0 && leaf->type == LEAF_OR)
		leaf->right->executed = true;
	else if (ms->exit_code > 0 && leaf->type == LEAF_AND)
		leaf->right->executed = true;
}

void	exec_pipe(t_leaf *leaf)
{
	int	fd[2];

	if (leaf->parent && leaf->parent->operator == OP_PIPE)
	{
		leaf->left->fd_read[0] = leaf->fd_read[0];
		leaf->left->fd_read[1] = leaf->fd_read[1];
	}
	pipe(fd);
	leaf->left->fd_write[0] = fd[0];
	leaf->left->fd_write[1] = fd[1];
	leaf->right->fd_read[0] = fd[0];
	leaf->right->fd_read[1] = fd[1];
}

void	exec_redirect(t_leaf *leaf, t_minishell *ms)
{
	(void)ms;
	(void)leaf;
}

void	init_leaf_content(t_leaf *leaf)
{
	size_t	i;
	t_token	*token;

	i = 0;
	token = leaf->head_token;
	leaf->content = (char **)ft_calloc(leaf->size + 1, sizeof(char *));
	while (i < leaf->size)
	{
		leaf->content[i] = token->content;
		token = token->next;
		i++;
	}
}

void	exec_word(t_leaf *leaf, t_minishell *ms)
{
	char	*path;
	pid_t	child;

	child = fork();
	if (child == 0)
	{
		if (leaf->fd_write[0])
		{
			dup2(leaf->fd_write[1], STDOUT_FILENO);
			close(leaf->fd_write[0]);
		}
		if (leaf->fd_read[0])
		{
			dup2(leaf->fd_read[0], STDIN_FILENO);
			close(leaf->fd_read[1]);
		}
		// path = get_path(leaf->head_token->content);
		path = leaf->head_token->content;
		init_leaf_content(leaf);
		execve(path, leaf->content, ms->envp);
		if (leaf->fd_write[0])
			close(leaf->fd_write[1]);
		if (leaf->fd_read[0])
			close(leaf->fd_read[0]);
		exit (0);
	}
	if (leaf->fd_read[0])
	{
		printf("closing %d and %d\n", leaf->fd_read[0], leaf->fd_read[1]);
		close(leaf->fd_read[0]);
		close(leaf->fd_read[1]);
		printf("closed %d and %d\n", leaf->fd_read[0], leaf->fd_read[1]);
	}
}

void	exec_leaf(t_leaf *leaf, t_minishell *ms)
{
	// expand_content(leaf)
	if (leaf->operator == OP_LOGICAL)
		exec_logical(leaf, ms);
	else if (leaf->operator == OP_PIPE)
		exec_pipe(leaf);
	else if (leaf->operator == OP_REDIRECT)
		exec_redirect(leaf, ms);
	else if (leaf->operator == OP_NONE)
		exec_word(leaf, ms);
	leaf->executed = true;
}

void	start_executor(t_leaf *leaf, t_minishell *ms)
{
	while (leaf && !ms->error)
	{
		if (!leaf->executed)
			exec_leaf(leaf, ms);
		// if (!leaf->executed)
		// {
		// 	printf("executing %s\n", leaf->head_token->content);
		// 	leaf->executed = true;
		// }
		if (leaf->left && !leaf->left->executed)
			leaf = leaf->left;
		else if (leaf->right && !leaf->right->executed)
			leaf = leaf->right;
		else
			leaf = leaf->parent;
	}
	sleep(1);
}
