/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpaim-so <jpaim-so@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 17:49:57 by jpaim-so          #+#    #+#             */
/*   Updated: 2026/05/24 16:46:46 by jpaim-so         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*test(void *arg)
{
	printf("Testing threads!\n");
	sleep(1);
	printf("Ending thread!\n");
}

int	main(int argc, char *argv[])
{
	pthread_t	thread_1;
	pthread_t	thread_2;

	pthread_create(&thread_1, NULL, &test, NULL);
	pthread_create(&thread_2, NULL, &test, NULL);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	return (0);
}
