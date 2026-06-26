/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 19:42:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/26 02:32:55 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Lock both dongles in a consistent order and mark them as taken. */
static int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left;
	second = coder->right;
	if (first > second)
	{
		first = coder->right;
		second = coder->left;
	}
	pthread_mutex_lock(&first->lock);
	if (first != second)
		pthread_mutex_lock(&second->lock);
	first->status = 1;
	second->status = 1;
	return (1);
}

/* Check if both dongles are free and past cooldown, using consistent lock
  order. */
int	dongles_available(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	int			ok;
	long long	now;

	first = coder->left;
	second = coder->right;
	if (first > second)
	{
		first = coder->right;
		second = coder->left;
	}
	now = get_time_ms() - coder->sim->sim_start_time;
	ok = 1;
	pthread_mutex_lock(&first->lock);
	if (first->status != 0 || now < first->release_time
		+ coder->sim->dongle_cooldown)
		ok = 0;
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_lock(&second->lock);
	if (second->status != 0 || now < second->release_time
		+ coder->sim->dongle_cooldown)
		ok = 0;
	pthread_mutex_unlock(&second->lock);
	return (ok);
}

/* Register a dongle request, wait until the coder can proceed, and reserve
   them. */
int	acquire_dongles(t_coder *coder)
{
	int	status;

	while (1)
	{
		pthread_mutex_lock(&coder->sim->state_lock);
		status = scheduler_step(coder);
		pthread_mutex_unlock(&coder->sim->state_lock);
		if (status == 1)
			return (1);
		if (status == 0)
		{
			usleep(500);
			continue ;
		}
		if (dongles_available(coder))
		{
			take_dongles(coder);
			log_state(coder, "has taken a dongle");
			log_state(coder, "has taken a dongle");
			return (0);
		}
		usleep(500);
	}
}

/* Release both dongles and start their cooldown timer. */
void	release_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long long	now;

	first = coder->left;
	second = coder->right;
	if (first > second)
	{
		first = coder->right;
		second = coder->left;
	}
	now = get_time_ms() - coder->sim->sim_start_time;
	first->status = 0;
	second->status = 0;
	first->release_time = now;
	second->release_time = now;
	pthread_mutex_unlock(&second->lock);
	if (first != second)
		pthread_mutex_unlock(&first->lock);
}
