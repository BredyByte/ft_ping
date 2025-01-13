#include "defines.h"
#include "utils.h"
#include "args.h"
#include "ping_io.h"
#include "net_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

t_ping g_data;

int	main(int argc, char **argv) {

	if (getuid() != 0)
	{
		fprintf(stderr,"ft_ping: root privileges are required to run ft_ping.\n");
		return 1;
	}

	args(argc, argv);

	get_available_interface();

	init_ping();

	free_allocations();

	return 0;
}
