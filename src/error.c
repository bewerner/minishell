/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 19:50:28 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:06:02 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	put_syntax_error_line(t_input *syntax_error_input, t_minishell *ms)
{
	size_t	line_count;
	t_input	*input;

	if (!syntax_error_input)
		return ;
	line_count = 1;
	input = ms->head_input;
	while (input != syntax_error_input)
	{
		input = input->next;
		line_count++;
	}
	ft_putstr_fd("minishell: line ", STDERR_FILENO);
	ft_putnbr_fd(line_count, STDERR_FILENO);
	ft_putstr_fd(": `", STDERR_FILENO);
	ft_putstr_fd(syntax_error_input->content, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
}

void	put_error_prefix(t_token *token, t_minishell *ms)
{
	size_t	line;

	line = ms->line_count;
	if (token && ms->syntax_error != SYN_EOF)
		line = token->line;
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (!ms->interactive)
	{
		ft_putstr_fd("line ", STDERR_FILENO);
		ft_putnbr_fd(line, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
}

void	syntax_error(t_token *token, t_minishell *ms)
{
	put_error_prefix(token, ms);
	if (ms->syntax_error == SYN_EOF || !token)
		ft_putendl_fd("syntax error: unexpected end of file", STDERR_FILENO);
	else if (ms->syntax_error == SYN_NEWLINE)
		ft_putendl_fd("syntax error near unexpected token `newline'",
			STDERR_FILENO);
	else if (ms->syntax_error == SYN_OPERATOR)
	{
		ft_putstr_fd("syntax error near unexpected token `", STDERR_FILENO);
		ft_putstr_fd(token->content, STDERR_FILENO);
		ft_putendl_fd("'", STDERR_FILENO);
	}
	else if (ms->syntax_error == SYN_UNCLOSED)
	{
		ft_putstr_fd("unexpected EOF while looking for matching `",
			STDERR_FILENO);
		if (in_double_quotes(token->content, ft_strlen(token->content) - 1))
			ft_putendl_fd("\"'", STDERR_FILENO);
		else
			ft_putendl_fd("\''", STDERR_FILENO);
	}
	if (token)
		token->syntax_error = true;
	ms->syntax_error_input = input_last(ms->head_input);
	ms->exit_code = 258;
}

void	check_syntax(t_token *token, t_minishell *ms)
{
	while (token && !ms->syntax_error && !ms->error)
	{
		if (token->operator && token == ms->head_token
			&& token->operator >= OP_PIPE)
			ms->syntax_error = SYN_OPERATOR;
		else if (!token->next && ms->head_input
			&& ms->head_input->complete && is_unclosed(token->content))
			ms->syntax_error = SYN_UNCLOSED;
		else if (token->operator
			&& token->next && token->next->operator >= token->operator)
		{
			ms->syntax_error = SYN_OPERATOR;
			token = token->next;
		}
		else if (token->operator && !token->next && ms->head_input->complete)
			ms->syntax_error = SYN_EOF;
		else if (token->operator && !token->next
			&& token->operator == OP_REDIRECT)
			ms->syntax_error = SYN_NEWLINE;
		if (ms->syntax_error == SYN_NEWLINE)
			init_heredocs(ms->head_token, ms);
		if (ms->syntax_error)
			syntax_error(token, ms);
		token = token->next;
	}
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
	else if (errno)
	{
		perror(s1);
	}
	else
	{
		ft_putendl_fd(s1, STDERR_FILENO);
	}
	errno = 0;
	ms->error = true;
	ms->exit_code = exit_code;
}
