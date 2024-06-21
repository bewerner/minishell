/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 22:55:41 by bwerner           #+#    #+#             */
/*   Updated: 2024/06/21 17:25:43 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_leaf	*get_root(t_leaf *leaf)
{
	if (!leaf)
		return (NULL);
	while (leaf->next)
		leaf = leaf->next;
	while (leaf->prev)
	{
		if (leaf->operator == OP_LOGICAL)
			return (leaf);
		leaf = leaf->prev;
	}
	while (leaf->next)
	{
		if (leaf->operator == OP_PIPE)
			return (leaf);
		leaf = leaf->next;
	}
	while (leaf->prev)
		leaf = leaf->prev;
	return (leaf);
}

t_leaf	*get_first_leaf(t_leaf *leaf, t_leaf *root)
{
	if (root->operator != OP_LOGICAL)
		return (root);
	while (leaf->left && leaf->operator == OP_LOGICAL)
		leaf = leaf->left;
	return (leaf);
}

void	connect_logical_operators(t_leaf *root)
{
	t_leaf	*leaf;

	leaf = root->prev;
	while (leaf)
	{
		if (leaf->operator == OP_LOGICAL)
			connect_to_left_branch(root, leaf);
		leaf = leaf->prev;
	}
}

void	init_tree(t_minishell *ms)
{
	if (ms->error || !ms->head_leaf)
		return ;
	ms->root = get_root(ms->head_leaf);
	connect_logical_operators(ms->root);
	connect_pipes(ms->root, ms->head_leaf, ms->head_leaf);
	connect_rest(ms->head_leaf, ms->head_leaf, ms->root);
	ms->first_leaf = get_first_leaf(ms->root, ms->root);
}
