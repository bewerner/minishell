/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 19:50:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/12 20:54:26 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	put_syntax_error_line(t_input *input_syntax_error, t_minishell *ms)
{
	size_t	line_count;
	t_input	*input;

	line_count = 1;
	input = ms->head_input;
	while (input != input_syntax_error)
	{
		input = input->next;
		line_count++;
	}
	ft_putstr_fd("minishell: line ", STDERR_FILENO);
	ft_putnbr_fd(line_count, STDERR_FILENO);
	ft_putstr_fd(": `", STDERR_FILENO);
	ft_putstr_fd(input_syntax_error->content, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
}

void	put_error_prefix(t_minishell *ms)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (!ms->interactive)
	{
		ft_putstr_fd("line ", STDERR_FILENO);
		ft_putnbr_fd(ms->line_count, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
}

void	syntax_error(t_token *token, char *str, t_minishell *ms)
{
	if (!ms->syntax_error && token && is_unclosed(token->content))
	{
		put_error_prefix(ms);
		ft_putendl_fd("unexpected EOF while looking for matching `\"'", STDERR_FILENO);
	}
	else if (!ms->syntax_error && token)
	{
		put_error_prefix(ms);
		ft_putstr_fd("syntax error near unexpected token `", STDERR_FILENO);
		ft_putstr_fd(token->content, STDERR_FILENO);
		ft_putendl_fd("'", STDERR_FILENO);
	}
	else if (ms->syntax_error && str)
	{
		put_error_prefix(ms);
		ft_putstr_fd("syntax error near unexpected token `", STDERR_FILENO);
		ft_putstr_fd(str, STDERR_FILENO);
		ft_putendl_fd("'", STDERR_FILENO);
	}
	else if (ms->syntax_error && token && is_unclosed(token->content))
	{
		put_error_prefix(ms);
		ft_putendl_fd("syntax error: unexpected end of file", STDERR_FILENO);
	}
	if (token)
		token->syntax_error = true;
	ms->syntax_error = true;
	ms->input_syntax_error = input_last(ms->head_input);
	ms->exit_code = 258;
}

void	ms_error(char *s1, char *s2, int64_t exit_code, t_minishell *ms)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (s1 && s2)
	{
		ft_putstr_fd(s1, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(s2, STDERR_FILENO);
	}
	else if (errno && errno != ENOTTY)
	{
		perror(s1);
		// if (errno == EACCES)
		// 	exit_code = EXIT_FAILURE26;
	}
	else
	{
		ft_putendl_fd(s1, STDERR_FILENO);
	}
	errno = 0;
	ms->error = true;
	ms->exit_code = exit_code;
}
