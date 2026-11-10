/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 11:01:52 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/28 11:02:10 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../codexion.h"

static void	order_dongles(t_coder *coder, t_dongle **first, t_dongle **second)
{
	if (coder->id % 2 == 1)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

void	*coder_routin(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		smart_sleep(coder->config->time_to_compile, coder->shared);
	while (check_simulation_running(coder->shared))
	{
		if (take_dongles(coder) != 0)
			break ;
		execute_compilation(coder);
		execute_debugging(coder);
		execute_refactoring(coder);
	}
	return (NULL);
}

int	lock_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	coder->request_time = get_time_ms();
	req.coder_id = coder->id;
	req.key = request_key(coder);
	pthread_mutex_lock(&dongle->lock);
	heap_push(dongle, req);
	while (check_simulation_running(coder->shared))
	{
		if (can_take_dongle(dongle)
			&& heap_peek(dongle).coder_id == coder->id)
		{
			heap_pop(dongle);
			dongle->is_taken = 1;
			pthread_mutex_unlock(&dongle->lock);
			print_note(coder->shared, coder->id, "has taken a dongle");
			return (0);
		}
		pthread_mutex_unlock(&dongle->lock);
		usleep(200);
		pthread_mutex_lock(&dongle->lock);
	}
	pthread_mutex_unlock(&dongle->lock);
	return (1);
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	order_dongles(coder, &first, &second);
	if (lock_dongle(coder, first) != 0)
		return (1);
	if (lock_dongle(coder, second) != 0)
	{
		release_dongle(first, coder->config->dongle_cooldown);
		return (1);
	}
	return (0);
}

int	simulation_start(t_sim *sim)
{
	long	i;

	sim->shared.start_time = get_time_ms();
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coder[i].last_compilation = sim->shared.start_time;
		if (pthread_create(&sim->coder[i].thread,
				NULL, coder_routin, &sim->coder[i]) != 0)
			return (1);
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_rutine, sim) != 0)
		return (1);
	if (join_threads(sim) != 0)
		return (1);
	return (0);
}
