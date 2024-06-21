/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_word.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 23:42:53 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 01:41:00 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

void	exec_x(char *cmd, t_leaf *leaf, t_minishell *ms)
{
	ms->exit_code = EXIT_SUCCESS;
	if (ft_strncmp(cmd, "echo", 5) == 0)
		exec_echo(leaf, leaf->head_token->next);
	else if (ft_strncmp(cmd, "cd", 3) == 0)
		exec_cd(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "pwd", 4) == 0)
		exec_pwd(ms);
	else if (ft_strncmp(cmd, "export", 7) == 0)
		exec_export(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "unset", 6) == 0)
		exec_unset(leaf, leaf->head_token->next, ms);
	else if (ft_strncmp(cmd, "env", 4) == 0)
		exec_env(ms);
	else if (ft_strncmp(cmd, "exit", 5) == 0)
		exec_exit(leaf, leaf->head_token->next, ms);
	else
		exec_cmd(leaf, cmd, ms);
}

void	exec_word(t_leaf *leaf, t_minishell *ms)
{
	set_signal(SIGINT, sigint_handler_exec);
	set_signal(SIGQUIT, sigquit_handler_exec);
	if (leaf->fork)
		leaf->child_pid = fork();
	if (leaf->child_pid == -1)
		ms_error("exec_word: fork", NULL, EXIT_FAILURE, ms);
	else if (!leaf->child_pid || !leaf->fork)
	{
		errno = 0;
		set_signal(SIGQUIT, SIG_DFL);
		set_signal(SIGINT, SIG_DFL);
		close_fd_parent_child(false, true, ms);
		exec_x(leaf->head_token->content, leaf, ms);
		if (leaf->fork)
			terminate(ms->exit_code, ms);
	}
	close_fd_parent_child(true, false, ms);
	restore_std_fd(ms);
}
