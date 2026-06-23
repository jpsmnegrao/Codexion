/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 13:51:20 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/23 17:16:04 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Check whether any coder has exceeded the burnout timeout. */
static int	check_burnout(t_sim *sim)
{
	int			i;
	long long	time;

	i = 0;
	time = get_time_ms();
	while (i < sim->number_of_coders)
	{
		if (time - sim->coders[i].last_compile_start >= sim->time_to_burnout)
		{
			sim->stop = 1;
			log_state(&sim->coders[i], "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

/* Check whether every coder has completed the required number of compiles. */
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

/* Print a timestamped state change for one coder, protected by the output
   lock. */
void	log_state(t_coder *coder, const char *state)
{
	long long	current_time;

	current_time = get_time_ms() - coder->sim->sim_start_time;
	pthread_mutex_lock(&coder->sim->output_lock);
	printf("%lld %d %s\n", current_time, coder->id, state);
	pthread_mutex_unlock(&coder->sim->output_lock);
}

/* Monitor the simulation until a coder burns out or all required compiles are
   done. */
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

/* Set the coder's request time, using a shared startup timestamp for the first
   round. */
void	set_request_time(t_coder *coder)
{
	if (coder->request_initialized)
		coder->request_time = get_time_ms();
	coder->request_initialized = 1;
}
