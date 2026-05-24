/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:51 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/24 18:37:57 by jpaim-so         ###   ########.fr       */
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

#endif