/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:51 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/16 00:12:16 by jpaim-so         ###   ########.fr       */
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
typedef struct s_heap_container	t_heap_container;

typedef struct s_dongle
{
	int				status;
	long long		release_time;
}	t_dongle;

typedef struct s_sim
{
	long long			number_of_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	long long			number_of_compiles_required;
	long long			dongle_cooldown;
	int					scheduler;
	long long			sim_start_time;
	int					stop;
	pthread_mutex_t		output_lock;
	pthread_mutex_t		state_lock;
	pthread_cond_t		condition;
	t_coder				*coders;
	t_dongle			*dongles;
	t_heap_container	wait_queue;
}	t_sim;

typedef struct s_coder
{
	pthread_t		thread;
	int				id;
	int				times_compiled;
	long long		last_compile_start;
	long long		request_time;
	int				is_waiting;
	t_dongle		*left;
	t_dongle		*right;
	t_sim			*sim;
}	t_coder;

typedef struct s_heap_entry
{
	t_coder		*coder;
	long long	prio;
	int			tiebreaker;
}	t_heap_entry;

typedef struct s_heap_container
{
	t_heap_entry	*heap;
	int				size;
	int				capacity;
	int				scheduler;
}	t_heap_container;

// PARSING AND INITIALIZATION

int				parse_input(t_sim *sim, int argc, char *argv[]);
long long		get_time_ms(void);
int				setup_simulation(t_sim *sim);
void			destroy_simulation(t_sim *sim);

// SIMULATION

int				run_sim(t_sim *sim);
void			*coder_routine(void *arg);
void			log_state(t_coder *coder, const char *state);
int				acquire_dongles(t_coder *coder);
void			release_dongles(t_coder *coder);
int				dongles_available(t_coder *coder);
int				can_proceed(t_coder *coder);

// HEAP MANAGEMENT

int				smallest_child(t_heap_container *heap, int i);
void			heap_sift_up(t_heap_container *heap, int i);
int				heap_init(t_heap_container *s_heap_container, int capacity,
					int scheduler);
int				heap_insert(t_heap_container *s_heap_container, t_coder *coder,
					long long prio, int tiebreaker);
t_heap_entry	heap_extract_min(t_heap_container *heap);
void			heap_free(t_heap_container *s_heap_container);

// MONITORING

void			*monitor_routine(void *arg);

#endif