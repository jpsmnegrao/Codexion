/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:51:20 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/28 23:39:34 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_sim *sim)
{
	int			i;
	long long	time;

	i = 0;
	time = get_time_ms();
	while (i < sim->number_of_coders)
	{
		if (time - sim->coders[i].last_compile_start > sim->time_to_burnout)
		{
			sim->stop = 1;
			log_state(&sim->coders[i], "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_total_compiles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->coders[i].times_compiled < sim->number_of_compiles_required)
			return (0);
		i++;
	}
	sim->stop = 1;
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;

	sim = (t_sim *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->state_lock);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->state_lock);
			return (NULL);
		}
		if (check_burnout(sim))
		{
			pthread_mutex_unlock(&sim->state_lock);
			return (NULL);
		}
		if (check_total_compiles(sim))
		{
			pthread_mutex_unlock(&sim->state_lock);
			return (NULL);
		}
		pthread_mutex_unlock(&sim->state_lock);
		usleep(500);
	}
	return (NULL);
}
