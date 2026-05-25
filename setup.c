/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 22:09:37 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/25 13:38:49 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	destroy_simulation(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].lock);
		pthread_cond_destroy(&sim->dongles[i].condition);
		i++;
	}
	pthread_mutex_destroy(&sim->output_lock);
	pthread_mutex_destroy(&sim->state_lock);
	free(sim->dongles);
	free(sim->coders);
}

static int	init_mutexes(t_sim *sim)
{
	int	i;

	if (pthread_mutex_init(&sim->output_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->state_lock, NULL) != 0)
		return (pthread_mutex_destroy(&sim->output_lock), 1);
	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].lock, NULL) != 0)
			return (destroy_simulation(sim, i), 1);
		if (pthread_cond_init(&sim->dongles[i].condition, NULL) != 0)
			return (pthread_mutex_destroy(&sim->dongles[i].lock),
				destroy_simulation(sim, i), 1);
		i++;
	}
	return (0);
}

static void	init_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1) % sim->number_of_coders];
		sim->coders[i].last_compile_start = sim->sim_start_time;
		i++;
	}
}

int	setup_simulation(t_sim *sim)
{
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (1);
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (free(sim->dongles), 1);
	sim->stop = 0;
	if (init_mutexes(sim) != 0)
		return (1);
	sim->sim_start_time = get_time_ms();
	init_coders(sim);
	return (0);
}
