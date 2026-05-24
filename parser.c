/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 17:52:08 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/24 18:42:10 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	parse_positive_int(char *str, long long *out)
{
	long long	num;
	int			i;

	i = 0;
	num = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		num = num * 10 + (str[i] - '0');
		if (num > INT_MAX)
			return (0);
		i++;
	}
	*out = num;
	return (1);
}

int	parse_input(t_sim *sim, int argc, char *argv[])
{
	if (argc != 9)
		return (printf("Incorrect number of arguments provided!\n"), 0);
	if (!parse_positive_int(argv[1], &sim->number_of_coders)
		|| !parse_positive_int(argv[2], &sim->time_to_burnout)
		|| !parse_positive_int(argv[3], &sim->time_to_compile)
		|| !parse_positive_int(argv[4], &sim->time_to_debug)
		|| !parse_positive_int(argv[5], &sim->time_to_refactor)
		|| !parse_positive_int(argv[6], &sim->number_of_compiles_required)
		|| !parse_positive_int(argv[7], &sim->dongle_cooldown))
		return (printf("Invalid argument!\n"), 0);
	if (sim->number_of_coders < 1)
		return (printf("Need at least 1 coder!\n"), 0);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = 0;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = 1;
	else
		return (printf("Incorrect scheduler provided!\n"), 0);
	return (1);
}
