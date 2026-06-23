/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 17:52:08 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/06/23 14:12:06 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Convert a positive decimal string to a long long value.
   Rejects empty strings, non-digit characters, and overflow beyond INT_MAX. */
static int	parse_positive_int(char *str, long long *out)
{
	long long	num;
	int			i;

	i = 0;
	num = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		num = num * 10 + (str[i] - '0');
		if (num > INT_MAX)
			return (1);
		i++;
	}
	*out = num;
	return (0);
}

/* Parse and validate all command-line arguments for the simulation.
   Also converts the scheduler string into the internal enum-like value. */
int	parse_input(t_sim *sim, int argc, char *argv[])
{
	if (argc != 9)
		return (printf("Incorrect number of arguments provided!\n"), 1);
	if (parse_positive_int(argv[1], &sim->number_of_coders)
		|| parse_positive_int(argv[2], &sim->time_to_burnout)
		|| parse_positive_int(argv[3], &sim->time_to_compile)
		|| parse_positive_int(argv[4], &sim->time_to_debug)
		|| parse_positive_int(argv[5], &sim->time_to_refactor)
		|| parse_positive_int(argv[6], &sim->number_of_compiles_required)
		|| parse_positive_int(argv[7], &sim->dongle_cooldown))
		return (printf("Invalid argument!\n"), 1);
	if (sim->number_of_coders < 1)
		return (printf("Need at least 1 coder!\n"), 1);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = 0;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = 1;
	else
		return (printf("Incorrect scheduler provided!\n"), 1);
	return (0);
}
