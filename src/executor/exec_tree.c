/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 20:23:20 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/21 23:21:48 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	wait_for_child_processes(t_minishell *ms)
{
	t_leaf	*leaf;
	int		status;

	leaf = ms->head_leaf;
	while (leaf)
	{
		if (leaf->child_pid)
		{
			waitpid(leaf->child_pid, &status, 0);
			if (WIFSIGNALED(status))
				ms->exit_code = 127 + WTERMSIG(status); // ????????? double check this. temp
			else if (WIFEXITED(status))
				ms->exit_code = WEXITSTATUS(status);
			// printf("exit status of %s is %d\n", leaf->head_token->content, ms->exit_code);
			leaf->child_pid = 0;
		}
		leaf = leaf->next;
	}
	if (ms->debug)
		printf("exiting wait_for_child_processes\n");
}

void	exec_logical(t_leaf *leaf, t_minishell *ms)
{
	wait_for_child_processes(ms);
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
		leaf->left->read_pipe[0] = leaf->read_pipe[0];
		leaf->left->read_pipe[1] = leaf->read_pipe[1];
	}
	pipe(fd);
	leaf->left->write_pipe[0] = fd[0];
	leaf->left->write_pipe[1] = fd[1];
	leaf->right->read_pipe[0] = fd[0];
	leaf->right->read_pipe[1] = fd[1];
}

void	exec_leaf(t_leaf *leaf, t_minishell *ms)
{
	expand_leaf(leaf, ms);
	if (ms->error)
		return ;
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

void	exec_tree(t_leaf *leaf, t_minishell *ms)
{
	while (leaf && !ms->error)
	{
		if (!leaf->executed)
			exec_leaf(leaf, ms);
		if (leaf->left && !leaf->left->executed)
			leaf = leaf->left;
		else if (leaf->right && !leaf->right->executed)
			leaf = leaf->right;
		else
			leaf = leaf->parent;
	}
	wait_for_child_processes(ms);
}

		// if (!leaf->executed)
		// {
		// 	printf("executing %s\n", leaf->head_token->content);
		// 	leaf->executed = true;
		// }
