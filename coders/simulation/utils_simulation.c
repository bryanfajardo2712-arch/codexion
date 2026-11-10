/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.simulation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 12:58:53 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/30 12:58:59 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

void	smart_sleep(long duration_ms, t_shared *shared)
{
	long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < duration_ms)
	{
		if (!check_simulation_running(shared))
			break ;
		usleep(200);
	}
}

void	execute_debugging(t_coder *coder)
{
	print_note(coder->shared, coder->id, "is debugging");
	smart_sleep(coder->config->time_to_debug, coder->shared);
}

void	execute_refactoring(t_coder *coder)
{
	print_note(coder->shared, coder->id, "is refactoring");
	smart_sleep(coder->config->time_to_refactor, coder->shared);
}

void	updated_coder_compilation(t_coder *coder)
{
	pthread_mutex_lock(&coder->data_lock);
	coder->last_compilation = get_time_ms();
	coder->number_compilation++;
	pthread_mutex_unlock(&coder->data_lock);
}

void	execute_compilation(t_coder *coder)
{
	updated_coder_compilation(coder);
	print_note(coder->shared, coder->id, "is compiling");
	smart_sleep(coder->config->time_to_compile, coder->shared);
	release_dongles(coder);
}
