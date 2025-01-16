#include "defines.h"
#include "utils.h"
#include "args.h"
#include "ping_io.h"
#include "net_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

t_ping			g_data;
volatile bool	g_continue_ping = true;

static void	handle_interrupt(int sig)
{
    (void)sig;
    g_continue_ping = false;  // For interrupt ping-lifecycle
}


int	main(int argc, char **argv) {

	if (getuid() != 0)
	{
		fprintf(stderr,"ft_ping: root privileges are required to run ft_ping.\n");
		return 1;
	}

	if (signal(SIGINT, handle_interrupt) == SIG_ERR)
		exit_failure("signal");

	args(argc, argv);

	get_available_interface();

	init_ping();

	free_allocations();

	return 0;
}
