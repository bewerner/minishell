/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 21:25:47 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 20:59:47 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sigint_handler_heredoc(int signum)
{
	g_signal = signum;
	ioctl(STDIN_FILENO, TIOCSTI, "\n");
	printf("\033[A");
	ioctl(STDIN_FILENO, TIOCSTI, "\4");
}

void	sigquit_handler_exec(int signum)
{
	g_signal = signum;
}

void	sigint_handler_exec(int signum)
{
	g_signal = signum;
}

void	sigint_handler(int signum)
{
	g_signal = signum;
	rl_replace_line("", 0);
	ioctl(STDIN_FILENO, TIOCSTI, "\n");
}

void	set_signal(int signum, void (*handler_function)(int))
{
	struct sigaction	sa;

	sa.sa_handler = handler_function;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(signum, &sa, NULL);
}
