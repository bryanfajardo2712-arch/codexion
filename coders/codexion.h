/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bryfajar <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 13:00:05 by bryfajar          #+#    #+#             */
/*   Updated: 2026/10/29 12:05:33 by bryfajar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>
# include <limits.h>

typedef enum e_decision
{
	FIFO,
	EDF
}	t_decision;

typedef struct s_config
{
	long		number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	long		number_of_compiles_required;
	long		dongle_cooldown;
	t_decision	scheduler;
}	t_config;

typedef struct s_request
{
	int				coder_id;
	long			key;
}	t_request;

typedef struct s_dongle
{
	int				is_taken;
	long			free_ms;
	long			early_request;
	t_request		queue[2];
	int				queue_size;
	pthread_mutex_t	lock;
}	t_dongle;

typedef struct s_shared
{
	pthread_mutex_t	signal_lock;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	data_lock;
	long			start_time;
	int				signal;
}	t_shared;

typedef struct s_coder
{
	int				id;
	long			number_compilation;
	long			last_compilation;
	long			request_time;
	pthread_mutex_t	data_lock;
	pthread_t		thread;
	t_shared		*shared;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_config		*config;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	t_shared		shared;
	t_dongle		*dongles;
	t_coder			*coder;
	pthread_t		monitor;
}	t_sim;

int			init_shared(t_shared *shared);
int			parse(t_config *config, char **argv);
// dongles
void		destroy_dongles(t_dongle *dongle, long count);
t_dongle	*create_dongle(t_config *config);
// coders
int			init_coders(t_sim *sim);
int			data_init(t_sim *sim);
int			init_shared(t_shared *shared);
//utils
long		get_time_ms(void);
void		print_note(t_shared *shared, int id, char *text);
int			can_take_dongle( t_dongle *dongle);
// simulation
void		*coder_routin(void *arg);
int			take_dongles(t_coder *coder);
int			simulation_start(t_sim *sim);
int			lock_dongle(t_coder *coder, t_dongle *dongle);

// utils simulation
void		smart_sleep(long duration_ms, t_shared *shared);
void		execute_debugging(t_coder *coder);
void		execute_refactoring(t_coder *coder);
void		execute_compilation(t_coder *coder);
void		updated_coder_compilation(t_coder *coder);
// utils simulation 2
void		release_dongle(t_dongle *dongle, long cooldown);
void		release_dongles(t_coder *coder);
int			join_threads(t_sim *sim);
long		request_key(t_coder *coder);
// monitor
int			check_simulation_running(t_shared *shared);
void		*monitor_rutine(void *arg);
int			check_coder_burnout(t_coder *coder);
void		print_burnout(t_shared *shared, int id);
// heap
void		heap_push(t_dongle *dongle, t_request req);
t_request	heap_peek(t_dongle *dongle);
void		heap_pop(t_dongle *dongle);
// clean_simulation
void		clean_simulation(t_sim *sim);
#endif
