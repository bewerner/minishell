/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tree.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:11:30 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/09 02:03:59 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// 256 chars / 16

size_t	get_level(t_leaf *leaf)
{
	size_t level;

	level = 0;
	while (leaf->parent)
	{
		leaf = leaf->parent;
		level++;
	}
	return (level);
}

size_t	get_width(t_leaf *leaf, size_t level)
{
	int	width;

	width = level;
	while (leaf->parent)
	{
		if (leaf->parent->left == leaf)
			width--;
		else
			width++;
		leaf = leaf->parent;
	}
	if (width < 0)
		width = 0;
	return ((size_t)width / 2);
}

void	print_leaf(t_leaf *leaf, size_t level)
{
	t_token			*token;
	size_t			i;
	size_t			width;
	static size_t	latest_level;
	static size_t	latest_width;

	i = 0;
	width = get_width(leaf, level);
	if (level > latest_level)
	{
		latest_level = level;
		latest_width = 0;
		if (width)
			printf("%30s", " ");
	}
	while (latest_width + 1 < width)
	{
		printf("%30s", " ");
		latest_width++;
	}
	latest_width = width;
	token = leaf->head_token;
	while (i < leaf->size)
	{
		printf("%s ", token->content);
		token = token->next;
		i++;
	}
	printf("%25s", " ");
	if (!leaf->next)
	{
		latest_level = 0;
		latest_width = 0;
	}
}

void	print_level(size_t level, t_leaf *root, t_leaf *leaf)
{
	(void)root;
	size_t	offset;

	offset = 120;
	offset -= level * 10;
	if (offset > 120)
		return ;
	while (offset > 0)
	{
		printf(" ");
		offset--;
	}
	while (leaf)
	{
		if (get_level(leaf) == level)
			print_leaf(leaf, level);
		leaf = leaf->next;
	}
	printf("\n\n\n");
}

void	debug_print_tree(t_leaf *root, t_leaf *head)
{
	size_t	i;

	printf("\n\n");
	i = 0;
	while (i < 20)
	{
		print_level(i, root, head);
		i++;
	}
}
