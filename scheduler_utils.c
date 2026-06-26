/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 15:48:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/26 02:30:49 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Compute the priority used to order a coder in the waiting queue.
   FIFO uses the request time, while EDF uses the burnout deadline. */
static long long	prio_calculation(t_coder *coder)
{
	if (coder->sim->scheduler == 0)
		return (coder->request_time);
	return (coder->last_compile_start + coder->sim->time_to_burnout);
}

/* Check whether another coder waiting for the same dongle has higher 
   priority. */
static int	loses_to_contender(t_coder *coder, t_dongle *dongle)
{
	t_heap_container	*q;
	t_heap_entry		*self;
	int					i;

	q = &coder->sim->wait_queue;
	self = NULL;
	i = 0;
	while (i < q->size)
	{
		if (q->heap[i].coder == coder)
			self = &q->heap[i];
		i++;
	}
	if (!self)
		return (0);
	i = 0;
	while (i < q->size)
	{
		if (q->heap[i].coder != coder && (q->heap[i].coder->left == dongle
				|| q->heap[i].coder->right == dongle) && has_priority(
				q->heap[i], *self))
			return (1);
		i++;
	}
	return (0);
}

/* Decide whether the coder may proceed and take dongles now. */
int	can_proceed(t_coder *coder)
{
	if (loses_to_contender(coder, coder->left))
		return (0);
	if (loses_to_contender(coder, coder->right))
		return (0);
	return (1);
}

/* Evaluate the coder's scheduling state:
   - return 1 if the simulation has stopped
   - return 0 if the coder must wait due to priority
   - return 2 if the coder may attempt to acquire dongles */
int	scheduler_step(t_coder *coder)
{
	if (coder->sim->stop)
		return (1);
	if (coder->left == coder->right)
		return (0);
	if (!coder->in_queue)
	{
		coder->request_time = get_time_ms();
		heap_insert(&coder->sim->wait_queue, coder,
			prio_calculation(coder), coder->id);
		coder->in_queue = 1;
	}
	if (!can_proceed(coder))
		return (0);
	heap_remove(&coder->sim->wait_queue, coder);
	coder->in_queue = 0;
	return (2);
}
