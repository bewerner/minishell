/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 20:23:20 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/16 21:21:06 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	temp(int signum)
{
	(void)signum;
	// write(2, "Quit: 3\n", 8);
}

void	wait_for_child_processes(t_minishell *ms)
{
	t_leaf	*leaf;
	int		status;

	status = 0; // this is needed so we don't read uninitialized status after teh while loop
	leaf = ms->head_leaf;
	while (leaf)
	{
		if (leaf->child_pid < 0)
			ms->exit_code = EXIT_FAILURE;
		else if (leaf->child_pid)
		{
			// signal(SIGQUIT, temp);
			waitpid(leaf->child_pid, &status, 0);
			// set_signal(SIGQUIT, SIG_IGN);
				// ms->exit_code = 127 + WTERMSIG(status); // ????????? double check this. temp
			if (WIFSIGNALED(status))
			{
				// fprintf(stderr, "wtermsig: %d\n", WTERMSIG(status));
				// fprintf(stderr, "g_signal: %d\n", g_signal);
				// ms->exit_code = 128 + g_signal; // ????????? double check this. temp
				ms->exit_code = 128 + WTERMSIG(status); // ????????? double check this. temp
				// printf("WIFSIGNALED exit code: %d", ms->exit_code);
			}
			else if (WIFEXITED(status))
			{
				ms->exit_code = WEXITSTATUS(status);
				// printf("WIFEXITED exit code: %d", ms->exit_code);
			}
			// printf("exit status of %s is %d\n", leaf->head_token->content, ms->exit_code);
			leaf->child_pid = 0;
		}
		leaf = leaf->next;
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGQUIT)
		write(STDOUT_FILENO, "Quit: 3\n", 8);
	set_signal(SIGQUIT, SIG_IGN);
	set_signal(SIGINT, sigint_handler);
	if (ms->debug)
		printf("exiting wait_for_child_processes\n");
}

void	exec_logical(t_leaf *leaf, t_minishell *ms)
{
	dup2(ms->fd_stdin_dup, STDIN_FILENO);
	dup2(ms->fd_stdout_dup, STDOUT_FILENO);
	wait_for_child_processes(ms);
	ms->in_pipeline = false;
	// printf("ec: %ld\n", ms->exit_code);
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
		// ms->close_in_child = p[1]; // already closed
		ms->close_in_child = -1;
		ms->close_in_parent[0] = p[0];
		leaf->executed = true;
		i = 0;
	}
}

// void	exec_pipe(t_leaf *leaf)
// {
// 	int	fd[2];

// 	if (leaf->parent && leaf->parent->operator == OP_PIPE)
// 	{
// 		leaf->left->read_pipe[0] = leaf->read_pipe[0];
// 		leaf->left->read_pipe[1] = leaf->read_pipe[1];
// 	}
// 	pipe(fd);
// 	leaf->left->write_pipe[0] = fd[0];
// 	leaf->left->write_pipe[1] = fd[1];
// 	leaf->right->read_pipe[0] = fd[0];
// 	leaf->right->read_pipe[1] = fd[1];
// }

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
		// debug_print_tokens(&ms->head_token, 1);
		// debug_print_leafs(&ms->head_leaf);
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
