/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:51 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/27 22:54:04 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <pthread.h>
# include <limits.h>
# include <string.h>
# include <sys/time.h>

typedef struct s_coder	t_coder;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	condition;
}	t_dongle;

typedef struct s_sim
{
	long long		number_of_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	long long		number_of_compiles_required;
	long long		dongle_cooldown;
	int				scheduler;
	long long		sim_start_time;
	int				stop;
	pthread_mutex_t	output_lock;
	pthread_mutex_t	state_lock;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_sim;

typedef struct s_coder
{
	pthread_t		thread;
	int				id;
	int				times_compiled;
	long long		last_compile_start;
	t_dongle		*left;
	t_dongle		*right;
	t_sim			*sim;
}	t_coder;

// PARSING AND INITIALIZATION

int			parse_input(t_sim *sim, int argc, char *argv[]);
long long	get_time_ms(void);
int			setup_simulation(t_sim *sim);
void		destroy_simulation(t_sim *sim, int count);

// SIMULATION

int			run_sim(t_sim *sim);
void		*coder_routine(void *arg);
void		log_state(t_coder *coder, const char *state);

#endif