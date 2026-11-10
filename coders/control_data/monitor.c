/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 17:50:45 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/30 17:50:48 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

static int	chek_all_copilations(t_sim *sim)
{
	long	i;
	long	required;
	int		finished_count;

	required = sim->config.number_of_compiles_required;
	if (required == -1)
		return (0);
	i = 0;
	finished_count = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coder[i].data_lock);
		if (sim->coder[i].number_compilation >= required)
			finished_count++;
		pthread_mutex_unlock(&sim->coder[i].data_lock);
		i++;
	}
	if (finished_count == sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->shared.signal_lock);
		sim->shared.signal = 1;
		pthread_mutex_unlock(&sim->shared.signal_lock);
		return (1);
	}
	return (0);
}

int	check_simulation_running(t_shared *shared)
{
	int	running;

	pthread_mutex_lock(&shared->signal_lock);
	running = (shared->signal == 0);
	pthread_mutex_unlock(&shared->signal_lock);
	return (running);
}

void	print_burnout(t_shared *shared, int id)
{
	pthread_mutex_lock(&shared->log_lock);
	if (check_simulation_running(shared))
	{
		printf("%ld %d burned out\n",
			get_time_ms() - shared->start_time, id);
		pthread_mutex_lock(&shared->signal_lock);
		shared->signal = 1;
		pthread_mutex_unlock(&shared->signal_lock);
	}
	pthread_mutex_unlock(&shared->log_lock);
}

int	check_coder_burnout(t_coder *coder)
{
	long	last_comp;
	long	now;

	now = get_time_ms();
	pthread_mutex_lock(&coder->data_lock);
	last_comp = coder->last_compilation;
	pthread_mutex_unlock(&coder->data_lock);
	if (now - last_comp <= coder->config->time_to_burnout)
		return (0);
	print_burnout(coder->shared, coder->id);
	return (1);
}

void	*monitor_rutine(void *arg)
{
	t_sim	*sim;
	long	i;

	sim = (t_sim *)arg;
	while (check_simulation_running(&sim->shared))
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			if (check_coder_burnout(&sim->coder[i]))
				return (NULL);
			i++;
		}
		if (chek_all_copilations(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
