/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 20:23:20 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/18 19:03:26 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	wait_for_child_processes(t_leaf *current, t_minishell *ms)
{
	t_leaf	*leaf;
	int		child_status;

	child_status = 0; // this is needed so we don't read uninitialized child_status after teh while loop
	leaf = ms->head_leaf;
	while (leaf && leaf != current)
	{
		if (leaf->exit_code != -1 && leaf->operator == OP_REDIRECT)
			ms->exit_code = leaf->exit_code;
		else if (leaf->child_pid < 0)
			ms->exit_code = EXIT_FAILURE;
		else if (leaf->child_pid)
		{
			waitpid(leaf->child_pid, &child_status, 0);
			if (WIFSIGNALED(child_status))
				ms->exit_code = 128 + WTERMSIG(child_status);
			else if (WIFEXITED(child_status))
				ms->exit_code = WEXITSTATUS(child_status);
		}
		leaf->child_pid = -1;
		leaf->exit_code = -1;
		leaf = leaf->next;
	}
	if (WIFSIGNALED(child_status) && WTERMSIG(child_status) == SIGQUIT)
		write(STDOUT_FILENO, "Quit: 3\n", 8);
	set_signal(SIGQUIT, SIG_IGN);
	set_signal(SIGINT, sigint_handler);
}

void	exec_logical(t_leaf *leaf, t_minishell *ms)
{
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
	wait_for_child_processes(leaf, ms);
	ms->in_pipeline = false;
	if (ms->exit_code == 0 && leaf->type == LEAF_OR)
		leaf->right->executed = true;
	else if (ms->exit_code > 0 && leaf->type == LEAF_AND)
		leaf->right->executed = true;
}

void	exec_pipe(t_leaf *leaf, t_minishell *ms)
{
	static size_t	i;
	static int		p[2];

	i++;
	ms->in_pipeline = true;
	if (i == 1)
	{
		pipe(p);
		dup2(p[1], STDOUT_FILENO);
		ms->close_in_child = p[0];
		ms->close_in_parent[0] = p[1];
	}
	if (i == 2)
	{
		ms->close_in_parent[1] = p[0];
		dup2(ms->fd_stdout_dup, STDOUT_FILENO);
		dup2(p[0], STDIN_FILENO);
		ms->close_in_child = -1;
		ms->close_in_parent[0] = p[0];
		leaf->executed = true;
		i = 0;
	}
}

bool	is_builtin(char *cmd)
{
	if (!cmd)
		return (false);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (true);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (true);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (true);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (true);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (true);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (true);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (true);
	return (false);
}

void	exec_leaf(t_leaf *leaf, t_minishell *ms)
{
	if (ms->error)
		return ;
	leaf->fork = ms->in_pipeline;
	if (!leaf->fork && leaf->operator == OP_NONE)
		leaf->fork = !is_builtin(leaf->head_token->content);
	if (leaf->operator == OP_LOGICAL)
		exec_logical(leaf, ms);
	else if (leaf->operator == OP_PIPE)
		exec_pipe(leaf, ms);
	else if (leaf->operator == OP_REDIRECT)
		exec_redirect(leaf, ms);
	else if (leaf->operator == OP_NONE)
		exec_word(leaf, ms);
	if (leaf->operator != OP_PIPE)
		leaf->executed = true;
}

void	exec_tree(t_leaf *leaf, t_minishell *ms)
{
	while (leaf && !ms->error)
	{
		if (!leaf->executed)
			expand_leaf(leaf, ms);
		if (!leaf->executed)
			exec_leaf(leaf, ms);
		if (leaf->left && !leaf->left->executed)
			leaf = leaf->left;
		else if (leaf->right && !leaf->right->executed)
			leaf = leaf->right;
		else
			leaf = leaf->parent;
	}
	wait_for_child_processes(NULL, ms);
}
