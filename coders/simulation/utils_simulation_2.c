/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_simulation_2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:09:12 by bryfajar          #+#    #+#             */
/*   Updated: 2026/11/03 14:09:19 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

long	request_key(t_coder *coder)
{
	if (coder->config->scheduler == FIFO)
		return (coder->request_time);
	return (coder->last_compilation + coder->config->time_to_burnout);
}

int	join_threads(t_sim *sim)
{
	long	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_join(sim->coder[i].thread, NULL) != 0)
			return (1);
		i++;
	}
	if (pthread_join(sim->monitor, NULL) != 0)
		return (1);
	return (0);
}

void	release_dongle(t_dongle *dongle, long cooldown)
{
	pthread_mutex_lock (&dongle->lock);
	dongle->is_taken = 0;
	dongle->free_ms = get_time_ms() + cooldown;
	pthread_mutex_unlock(&dongle->lock);
}

void	release_dongles(t_coder *coder)
{
	release_dongle(coder->left_dongle, coder->config->dongle_cooldown);
	release_dongle(coder->right_dongle, coder->config->dongle_cooldown);
}
