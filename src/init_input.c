/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 23:53:55 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/20 21:33:39 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_inputs(t_input **head)
{
	t_input	*ptr;
	t_input	*next;

	ptr = *head;
	while (ptr)
	{
		next = ptr->next;
		free(ptr->content);
		free(ptr);
		ptr = next;
	}
	*head = NULL;
}

void	check_syntax(t_token *token, t_minishell *ms)
{
	while (token && !ms->syntax_error && !ms->error)
	{
		if (token->operator && token == ms->head_token && token->operator >= OP_PIPE) // input starts with pipe/logical -> unexpected token
			ms->syntax_error = SYN_OPERATOR;
		else if (!token->next && ms->head_input
			&& ms->head_input->complete && is_unclosed(token->content)) // input is unclosed -> unexpected EOF looking for matching "
			ms->syntax_error = SYN_UNCLOSED;
		else if (token->operator
			&& token->next && token->next->operator >= token->operator) // input has an operator following an operator -> unexpected token
		{
			ms->syntax_error = SYN_OPERATOR;
			token = token->next;
		}
		else if (token->operator && !token->next && ms->head_input->complete) // last token is operator -> unexpected EOF
			ms->syntax_error = SYN_EOF;
		else if (token->operator && !token->next && token->operator == OP_REDIRECT) // last token is empty redirect -> unexpected newline
			ms->syntax_error = SYN_NEWLINE;
		if (ms->syntax_error)
			ms->syntax_error_input = input_last(ms->head_input);
		if (ms->syntax_error == SYN_NEWLINE)
			init_heredocs(ms->head_token, ms);
		if (ms->syntax_error)
			syntax_error(token, NULL, ms);
		token = token->next;
	}
}

// bool	check_syntax(t_token *token, t_minishell *ms)
// {
// 	if (token && token->operator >= OP_PIPE) // input starts with pipe/logical -> unexpected token
// 	{
// 		syntax_error(token, NULL, ms);
// 		return (false);
// 	}
// 	if (ms->head_input && ms->head_input->complete
// 		&& is_unclosed(token_last(ms->head_token)->content)) // input is unclosed -> unexpected EOF looking for matching "
// 	{
// 		syntax_error(token_last(ms->head_token), NULL, ms);
// 		return (false);
// 	}
// 	while (token)
// 	{
// 		if (token->operator)
// 		{
// 			if (token->next && token->next->operator >= token->operator) // input has an operator following an operator -> unexpected token
// 			{
// 				syntax_error(token->next, NULL, ms);
// 				return (false);
// 			}
// 			else if (!token->next && ms->head_input->complete) // last token is operator -> unexpected EOF
// 			{
// 				syntax_error(NULL, NULL, ms);
// 				return (false);
// 			}
// 			else if (!token->next && token->operator == OP_REDIRECT) // last token is empty redirect -> unexpected newline
// 			{
// 				syntax_error(NULL, "newline", ms);
// 				return (false);
// 			}
// 		}
// 		token = token->next;
// 	}
// 	if (ms->syntax_error)
// 		return (false);
// 	return (true);
// }

// void	check_syntax(t_token *token, t_minishell *ms)
// {
// 	while (token)
// 	{
// 		if (token->operator)
// 		{
// 			if (token == ms->head_token && token->operator >= OP_PIPE) // input starts with pipe/logical -> unexpected token
// 				syntax_error(token, NULL, ms);
// 			else if (token == ms->head_token && ms->head_input && ms->head_input->complete
// 				&& is_unclosed(token_last(ms->head_token)->content)) // input is unclosed -> unexpected EOF looking for matching "
// 			{
// 				syntax_error(token_last(ms->head_token), NULL, ms);
// 			}
// 			else if (token->next && token->next->operator >= token->operator) // input has an operator following an operator -> unexpected token
// 				syntax_error(token->next, NULL, ms);
// 			else if (!token->next && ms->head_input->complete) // last token is operator -> unexpected EOF
// 				syntax_error(NULL, NULL, ms);
// 			else if (!token->next && token->operator == OP_REDIRECT) // last token is empty redirect -> unexpected newline
// 				syntax_error(NULL, "newline", ms);
// 			else
// 			{
// 				token = token->next;
// 				continue;
// 			}
// 			break;
// 		}
// 		token = token->next;
// 	}
// }

bool	input_is_complete(t_input *input, t_token *token)
{
	(void)token;
	if (!input)
		return (false);
	if (input->complete)
		return (true);
	if (!token)
		return (true);
	if (token_last(token)->operator >= OP_PIPE)
		return (false);
	if (is_unclosed(token_last(token)->content))
		return (false);
	return (true);
}

char	*get_user_input(t_minishell *ms)
{
	char	*user_input;

	if (!ms->head_input)
		user_input = readline("minishell$ ");
	else
	{
		set_signal(SIGINT, sigint_handler_heredoc);
		user_input = readline("> ");
	}
	if (!user_input && errno) // malloc error
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	else if (!ms->head_input && !user_input) // ctrl + D (EOT)
	{
		printf("\033[Aminishell$ exit\n");
		terminate(ms->exit_code, ms);
	}
	else if (!user_input) // ctrl + D (EOT)
		ms->head_input->complete = true;
	else if (!ms->head_input && user_input && !user_input[0]) // enter (empty input)
	{
		free(user_input);
		user_input = NULL;
	}
	set_signal(SIGINT, sigint_handler);
	return (user_input);
}

void	store_input(char *content, t_minishell *ms)
{
	t_input *input;

	input = input_new(content);
	if (!input)
	{
		free(content);
		ms_error("store_input: input_new", NULL, EXIT_FAILURE, ms);
		return ;
	}
	input_add_back(&ms->head_input, input);
}

void	init_user_input(t_minishell *ms)
{
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		content = get_user_input(ms);
		if (!content)
			continue ;
		store_input(content, ms);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		init_heredocs(ms->head_token, ms);
	}
	check_syntax(ms->head_token, ms);
	init_heredocs(ms->head_token, ms);
	check_syntax(ms->head_token, ms);
}

char	*non_interactive_readline(t_minishell *ms)
{
	char	*line;

	line = get_next_line(STDIN_FILENO);
	ms->line_count++;
	// fprintf(stderr, "errno is %d\nec is %llu\nline: %s\n", errno, ms->exit_code, line);
	if (!line && errno) // malloc error
		ms_error("readline", NULL, EXIT_FAILURE, ms);
	if (line && ft_strrchr(line, '\n'))
		*ft_strrchr(line, '\n') = '\0';
	if (ms->error || (!line && !ms->head_input))
		terminate(ms->exit_code, ms);
	if (!line && ms->head_input)
		ms->head_input->complete = true;
	if (line && line[0] == '\0' && !ms->head_input)
	{
		free(line);
		line = NULL;
	}
	return (line);
}

void	init_noninteractive_input(t_minishell *ms)
{
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		content = non_interactive_readline(ms);
		if (!content)
			continue ;
		store_input(content, ms);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		init_heredocs(ms->head_token, ms);
	}
	check_syntax(ms->head_token, ms);
	init_heredocs(ms->head_token, ms);
	check_syntax(ms->head_token, ms);
	check_syntax(ms->head_token, ms);
}

// void	init_noninteractive_input(t_minishell *ms)
// {
// 	char	*content;

// 	while (!g_signal && !ms->error && !ms->syntax_error
// 		&& !input_is_complete(ms->head_input, ms->head_token))
// 	{
// 		content = non_interactive_readline(ms);
// 		if (!content)
// 		{
// 			if (!ms->head_input)
// 				terminate(ms->exit_code, ms);
// 			if (ms->head_token)
// 				check_syntax(ms->head_token, ms);
// 			ms->head_input->complete = true;
// 			continue ;
// 		}
// 		store_input(content, ms);
// 		init_tokens(ms);
// 		check_syntax(ms->head_token, ms);
// 		if (ms->head_token && (ms->syntax_error || !is_unclosed(token_last(ms->head_token)->content)))
// 			init_heredocs(ms->head_token, ms);
// 	}
// 	init_heredocs(ms->head_token, ms);
// 	check_syntax(ms->head_token, ms);
// 	check_syntax(ms->head_token, ms);
// 	if (ms->syntax_error && token_last(ms->head_token) && !is_unclosed(token_last(ms->head_token)->content)) // if unexpected end of file
// 		put_syntax_error_line(ms->syntax_error_input, ms);
// 	if (ms->syntax_error)
// 		terminate(ms->exit_code, ms);
// }

// void	init_input(t_minishell *ms)
// {
// 	if (ms->interactive)
// 	{
// 		init_user_input(ms);
// 		if (ms->line)
// 			add_history(ms->line);
// 	}
// 	else
// 	{
// 		init_noninteractive_input(ms);
// 		if (ms->syntax_error && token_last(ms->head_token) && !is_unclosed(token_last(ms->head_token)->content)) // if unexpected end of file
// 			put_syntax_error_line(ms->syntax_error_input, ms);
// 		// if (ms->syntax_error)
// 		// 	terminate(ms->exit_code, ms);
// 	}
// 	if (ms->syntax_error)
// 		ms->error = true;
// }

void	init_input(t_minishell *ms)
{
	char	*content;

	while (!g_signal && !ms->error && !ms->syntax_error
		&& !input_is_complete(ms->head_input, ms->head_token))
	{
		if (ms->interactive)
			content = get_user_input(ms);
		else
			content = non_interactive_readline(ms);
		if (!content)
			continue ;
		store_input(content, ms);
		init_tokens(ms);
		check_syntax(ms->head_token, ms);
		init_heredocs(ms->head_token, ms);
	}
	check_syntax(ms->head_token, ms);
	init_heredocs(ms->head_token, ms);
	if (ms->syntax_error == SYN_UNCLOSED)
		syntax_error(NULL, NULL, ms);
	if (ms->interactive && ms->line)
		add_history(ms->line);
	else if (!ms->interactive && ms->syntax_error && ms->syntax_error < SYN_EOF) // if unexpected end of file
		put_syntax_error_line(ms->syntax_error_input, ms);
}
