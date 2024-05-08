/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 22:55:41 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/08 02:29:51 by bwerner          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_leaf	*get_root(t_leaf *leaf)
{
	while (leaf->next)
		leaf = leaf->next;
	while (leaf->prev)
	{
		if (leaf->operator == OP_LOGICAL)
			return (leaf);
		leaf = leaf->prev;
	}
	return (leaf);
}

void	connect_to_right_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->right)
		branch = branch->right;
	branch->right = leaf;
}

void	connect_to_left_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->left)
		branch = branch->left;
	branch->left = leaf;
}

void	connect_logical_leafs(t_leaf *root)
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

void	connect_logical_branches(t_leaf *branch, t_leaf *head)
{
	t_leaf	*leaf;

	while (branch)
	{
		leaf = branch->next;
		if (!branch->left)
			leaf = head;
		while (leaf && leaf->operator != OP_LOGICAL)
		{
			if (leaf->operator == OP_PIPE)
			{
				if (!branch->left && branch->operator == OP_LOGICAL)
				{
					connect_to_left_branch(branch, leaf);
					branch = leaf;
				}
				else
					connect_to_right_branch(branch, leaf);
			}
			leaf = leaf->next;
		}
		branch = branch->left;
	}
}

void	connect_pipe_branches(t_leaf *branch, t_leaf *head)
{
	t_leaf	*leaf;

	while (branch)
	{
		leaf = branch->next;
		if (!branch->left)
			leaf = head;
		while (leaf && leaf->operator != OP_LOGICAL)
		{
			if (leaf->operator == OP_PIPE)
			{
				if (!branch->left && branch->operator == OP_LOGICAL)
				{
					connect_to_left_branch(branch, leaf);
					branch = leaf;
				}
				else
					connect_to_right_branch(branch, leaf);
			}
			leaf = leaf->next;
		}
		branch = branch->left;
	}
}

void	init_tree(t_minishell *ms)
{
	if (ms->error)
		return ;
	ms->root = get_root(ms->head_leaf);
	printf("root is: %s\n", ms->root->head_token->content);
	connect_logical_leafs(ms->root);
	connect_logical_branches(ms->root, ms->head_leaf);
	connect_pipe_branches(ms->root, ms->head_leaf);
}
