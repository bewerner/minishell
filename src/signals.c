/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 21:25:47 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/11 20:03:23 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler_heredoc(int signum)
{
	g_signal = signum;
	ioctl(0, TIOCSTI, "\n");
	printf("\033[A");
	ioctl(0, TIOCSTI, "\4");
}

void	sigquit_handler_exec(int signum)
{
	g_signal = signum;
	write(STDOUT_FILENO, "Quit: 3\n", 8);
}

void	sigint_handler_exec(int signum)
{
	g_signal = signum;
	write(1, "\n", 1);
}

void	sigint_handler(int signum)
{
	g_signal = signum;
	rl_replace_line("", 0);
	ioctl(0, TIOCSTI, "\n");
}

// void	sigint_handler(int signum)
// {
// 	g_signal = signum;
// 	write(1, "\n", 1);
// 	rl_replace_line("", 0);
// 	rl_on_new_line();
// 	rl_redisplay();
// }

void	set_signal(int signum, void (*handler_function)(int))
{
	struct sigaction	sa;

	sa.sa_handler = handler_function;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signum, &sa, NULL);
}

void	init_signals(t_minishell *ms)
{
	(void)ms;
	rl_catch_signals = 0;
	set_signal(SIGQUIT, SIG_IGN);
	set_signal(SIGINT, sigint_handler);
}
