/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 19:31:50 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/03 18:49:10 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal = 0;

// void	sigint_handler(int signum)
// {
// 	(void)signum;
// 	ioctl(0, TIOCSTI, "\4");
// }

int	main(int argc, char **argv, char **envp)
{
	t_minishell	ms;

	ft_bzero(&ms, sizeof(ms));
	ms.close_in_child = -1;
	ms.close_in_parent = -1;
	ms.fd_stdin_dup = dup(STDIN_FILENO);
	ms.fd_stdout_dup = dup(STDOUT_FILENO);
	init_signals(&ms);
	ms.envp = envp; // TEMP
	init_env(envp, &ms);
	ms.debug = 0;
	// if (argc != 1) // ?
	(void)argc;
	(void)argv;
	while (1)
	{
		cleanup(&ms);
		init_input(&ms);
		if (!ms.head_input || ms.error)
			continue ;
		rearrange_tokens(&ms);
		init_leafs(&ms);
		init_tree(&ms);
		exec_tree(ms.first_leaf, &ms);
	}
}

		// if (g_signal)
		// {
		// 	if (!ms.exit_code)
		// 		ms.exit_code = 1;
		// 	g_signal = 0;
		// }
