/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 16:35:51 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/21 16:36:00 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int	init_shared(t_shared *shared)
{
	shared->signal = 0;
	if (pthread_mutex_init (&shared->signal_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init (&shared->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy (&shared->signal_lock);
		return (1);
	}
	return (0);
}

static int	init_single_coder(t_sim *sim, long i)
{
	long	next_idx;

	next_idx = 0;
	sim->coder[i].id = i + 1;
	sim->coder[i].number_compilation = 0;
	sim->coder[i].last_compilation = 0;
	sim->coder[i].config = &sim->config;
	sim->coder[i].shared = &sim->shared;
	sim->coder[i].left_dongle = &sim->dongles[i];
	next_idx = (i + 1) % sim->config.number_of_coders;
	sim->coder[i].right_dongle = &sim->dongles[next_idx];
	if (pthread_mutex_init(&sim->coder[i].data_lock, NULL) != 0)
		return (1);
	return (0);
}

static void	destroy__coders_partial(t_coder *coders, long count)
{
	while (count > 0)
	{
		count --;
		pthread_mutex_destroy(&coders[count].data_lock);
	}
	free (coders);
}

int	init_coders(t_sim *sim)
{
	long	i;
	t_coder	*coders;

	i = 0;
	coders = malloc(sizeof(t_coder) * sim->config.number_of_coders);
	if (!coders)
		return (1);
	sim->coder = coders;
	while (i < sim->config.number_of_coders)
	{
		if (init_single_coder(sim, i) != 0)
		{
			destroy__coders_partial(sim->coder, i);
			return (1);
		}
		i++;
	}
	return (0);
}

int	data_init(t_sim *sim)
{
	if (init_shared(&sim->shared) != 0)
		return (1);
	sim->dongles = create_dongle(&sim->config);
	if (!sim->dongles)
		return (1);
	if (init_coders(sim) != 0)
		return (1);
	return (0);
}
