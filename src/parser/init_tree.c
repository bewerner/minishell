/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tree.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bwerner <bwerner@student.42heilbronn.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 22:55:41 by bwerner           #+#    #+#             */
/*   Updated: 2024/05/10 19:56:05 by bwerner          ###   ########.fr       */
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

void	connect_to_right_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->right)
		branch = branch->right;
	branch->right = leaf;
	leaf->parent = branch;
}

void	connect_to_left_branch(t_leaf *branch, t_leaf *leaf)
{
	while (branch->left)
		branch = branch->left;
	branch->left = leaf;
	leaf->parent = branch;
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

bool	leaf_is_behind_branch(t_leaf *leaf, t_leaf *branch)
{
	leaf = leaf->prev;
	while (leaf)
	{
		if (leaf == branch)
			return (true);
		leaf = leaf->prev;
	}
	return (false);
}

void	connect_pipes(t_leaf *branch, t_leaf *head)
{
	t_leaf	*leaf;

	while (branch->left)
		branch = branch->left;
	while (branch)
	{
		leaf = branch->next;
		if (!branch->left)
			leaf = head;
		while (leaf && leaf->operator != OP_LOGICAL)
		{
			if (leaf->operator == OP_PIPE && leaf != branch)
			{
				if (leaf_is_behind_branch(leaf, branch))
					connect_to_right_branch(branch, leaf);
				else
				{
					connect_to_left_branch(branch, leaf);
					branch = leaf;
				}
			}
			leaf = leaf->next;
			if (leaf == branch)
				leaf = leaf->next;
		}
		branch = branch->parent;
	}
}

void	connect_rest(t_leaf *head, t_leaf *branch, t_leaf *root)
{
	t_leaf	*leaf;

	if (root->operator < OP_PIPE)
	{
		leaf = head->next;
		while (leaf)
		{
			connect_to_left_branch(branch, leaf);
			leaf = leaf->next;
		}
		return ;
	}
	while (branch)
	{
		if (branch->operator >= OP_PIPE)
		{
			if (!branch->left)
			{
				if (!branch->parent || branch->parent->right != branch)
					leaf = head;
				else
					leaf = branch->parent->next;
				while (leaf && leaf->operator < OP_PIPE)
				{
					connect_to_left_branch(branch, leaf);
					leaf = leaf->next;
				}
			}
			if (!branch->right)
			{
				leaf = branch->next;
				if (leaf && leaf->operator < OP_PIPE)
				{
					connect_to_right_branch(branch, leaf);
					leaf = leaf->next;
				}
				while (leaf && leaf->operator < OP_PIPE)
				{
					connect_to_left_branch(branch, leaf);
					leaf = leaf->next;
				}
			}
		}
		branch = branch->next;
	}
}

void	init_tree(t_minishell *ms)
{
	if (ms->error)
		return ;
	ms->root = get_root(ms->head_leaf);
	if (ms->debug)
		printf("\nroot is: %s\n", ms->root->head_token->content);
	connect_logical_operators(ms->root);
	connect_pipes(ms->root, ms->head_leaf);
	connect_rest(ms->head_leaf, ms->head_leaf, ms->root);
	debug_print_tree(ms->root, ms->head_leaf);
	ms->first_leaf = get_first_leaf(ms->root, ms->root);
	if (ms->debug)
		printf("\nfirst leaf is: %s\n",
			ms->first_leaf->head_token->content);
	if (ms->debug && ms->first_leaf->left)
		printf("(left child: %s)\n",
			ms->first_leaf->left->head_token->content);
}
