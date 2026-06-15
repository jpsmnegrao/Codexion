/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 19:07:07 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/15 23:51:32 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sift_down(t_heap_container *heap, int i)
{
	t_heap_entry	temp;
	int				smallest;

	while (1)
	{
		smallest = smallest_child(heap, i);
		if (smallest == i)
			break ;
		temp = heap->heap[i];
		heap->heap[i] = heap->heap[smallest];
		heap->heap[smallest] = temp;
		i = smallest;
	}
}

int	heap_init(t_heap_container *s_heap_container, int capacity,
	int scheduler)
{
	s_heap_container->heap = malloc(sizeof(*s_heap_container->heap) * capacity);
	if (!s_heap_container->heap)
		return (0);
	s_heap_container->size = 0;
	s_heap_container->capacity = capacity;
	s_heap_container->scheduler = scheduler;
	return (1);
}

void	heap_free(t_heap_container *s_heap_container)
{
	if (!s_heap_container)
		return ;
	free(s_heap_container->heap);
	s_heap_container->heap = NULL;
	s_heap_container->size = 0;
	s_heap_container->capacity = 0;
}

int	heap_insert(t_heap_container *s_heap_container, t_coder *coder,
	long long prio, int tiebreaker)
{
	t_heap_entry	new_entry;

	if (!s_heap_container || !coder)
		return (0);
	if (s_heap_container->size >= s_heap_container->capacity)
		return (0);
	new_entry.coder = coder;
	new_entry.prio = prio;
	new_entry.tiebreaker = tiebreaker;
	s_heap_container->heap[s_heap_container->size] = new_entry;
	s_heap_container->size++;
	heap_sift_up(s_heap_container, s_heap_container->size - 1);
	return (1);
}

t_heap_entry	heap_extract_min(t_heap_container *heap)
{
	t_heap_entry	min;
	t_heap_entry	empty;

	if (heap->size == 0)
	{
		memset(&empty, 0, sizeof(t_heap_entry));
		return (empty);
	}
	min = heap->heap[0];
	heap->heap[0] = heap->heap[heap->size - 1];
	heap->size--;
	sift_down(heap, 0);
	return (min);
}
