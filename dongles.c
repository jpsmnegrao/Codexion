/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:42:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/26 18:14:49 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Lock both dongles in a consistent order and mark them as taken. */
int	take_dongles(t_coder *coder)
{
	coder->left->status = 1;
	coder->right->status = 1;
	return (1);
}

/* Check if both dongles are free and past cooldown, using consistent lock
  order. */
int	dongles_available(t_coder *coder)
{
	long long	now;

	if (coder->left == coder->right)
		return (0);
	now = get_time_ms() - coder->sim->sim_start_time;
	if (coder->left->status != 0)
		return (0);
	if (coder->left->release_time > 0 && now < coder->left->release_time
		+ coder->sim->dongle_cooldown)
		return (0);
	if (coder->right->status != 0)
		return (0);
	if (coder->right->release_time > 0 && now < coder->right->release_time
		+ coder->sim->dongle_cooldown)
		return (0);
	return (1);
}

/* Register a dongle request, wait until the coder can proceed, and reserve
   them. */
int	acquire_dongles(t_coder *coder)
{
	while (1)
	{
		pthread_mutex_lock(&coder->sim->state_lock);
		if (!coder->in_queue)
		{
			coder->request_time = get_time_ms();
			heap_insert(&coder->sim->wait_queue, coder,
				prio_calculation(coder), coder->id);
			coder->in_queue = 1;
		}
		if (coder->sim->stop)
		{
			pthread_mutex_unlock(&coder->sim->state_lock);
			return (1);
		}
		if (try_secure_resources(coder))
		{
			pthread_mutex_unlock(&coder->sim->state_lock);
			log_state(coder, "has taken a dongle");
			log_state(coder, "has taken a dongle");
			return (0);
		}
		pthread_mutex_unlock(&coder->sim->state_lock);
		usleep(500);
	}
}

/* Release both dongles and start their cooldown timer. */
void	release_dongles(t_coder *coder)
{
	long long	now;

	pthread_mutex_lock(&coder->sim->state_lock);
	now = get_time_ms() - coder->sim->sim_start_time;
	coder->left->status = 0;
	coder->right->status = 0;
	coder->left->release_time = now;
	coder->right->release_time = now;
	pthread_mutex_unlock(&coder->sim->state_lock);
}
