/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/26 17:34:28 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Synchronize the absolute start time of the simulation and initialize each
   individual coder's baseline compilation timestamp. */
static void	init_sim_timers(t_sim *sim)
{
	int	i;

	i = 0;
	sim->sim_start_time = get_time_ms();
	while (i < sim->number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->sim_start_time;
		i++;
	}
}

/* Safely flag the simulation to stop on thread failure, protecting 
   the shared state against data races. */
static void	handle_spawn_failure(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_lock);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->state_lock);
}

/* Launch the monitor thread and all coder threads, then wait for them to 
   finish. */
static int	run_sim(t_sim *sim)
{
	int			i;
	int			create_failed;
	pthread_t	monitor;

	i = 0;
	create_failed = 0;
	init_sim_timers(sim);
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
		return (1);
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			handle_spawn_failure(sim);
			create_failed = 1;
			break ;
		}
		i++;
	}
	while (i-- > 0)
		pthread_join(sim->coders[i].thread, NULL);
	pthread_join(monitor, NULL);
	return (create_failed);
}

/* Parse arguments, initialize the simulation, run it, then clean up
   resources. */
int	main(int argc, char *argv[])
{
	t_sim	sim;

	if (parse_input(&sim, argc, argv) != 0)
		return (1);
	if (setup_simulation(&sim) != 0)
		return (1);
	if (run_sim(&sim) != 0)
		return (destroy_simulation(&sim), 1);
	destroy_simulation(&sim);
	return (0);
}
