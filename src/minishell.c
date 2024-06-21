/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:20:18 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

void	init_signals(void)
{
	rl_catch_signals = 0;
	rl_terminal_name = "xterm-256color";
	set_signal(SIGQUIT, SIG_IGN);
	set_signal(SIGINT, sigint_handler);
}

void	init_fd(t_minishell *ms)
{
	ms->fd_stdin_dup = dup(STDIN_FILENO);
	ms->fd_stdout_dup = dup(STDOUT_FILENO);
	ms->close_in_child = -1;
	ms->close_in_parent[0] = -1;
	ms->close_in_parent[1] = -1;
	ms->close_in_parent[2] = -1;
}

void	check_args(int argc, char **argv)
{
	(void)argv;
	if (argc != 1)
	{
		ft_putendl_fd("No arguments allowed!", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	check_args(argc, argv);
	ft_bzero(&ms, sizeof(ms));
	init_fd(&ms);
	ms.interactive = isatty(STDIN_FILENO);
	errno = 0;
	init_signals();
	init_env(envp, &ms);
	while (1)
	{
		cleanup(&ms);
		init_input(&ms);
		if (ms.syntax_error && !ms.interactive)
			terminate(ms.exit_code, &ms);
		if (!ms.head_token || ms.error || ms.syntax_error)
			continue ;
		rearrange_tokens(&ms);
		init_leafs(&ms);
		init_tree(&ms);
		exec_tree(ms.first_leaf, &ms);
	}
}

	// ms.interactive = true;