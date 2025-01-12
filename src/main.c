#include "defines.h"
#include "utils.h"
#include "args.h"
#include "icmp.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

t_data global_data;

int	main(int argc, char **argv) {

	if (getuid() != 0)
	{
		fprintf(stderr,"Root privileges are required to run ft_ping.\n");
		return 1;
	}

	args(argc, argv);

	get_available_interface();

	print_args();

	start_ping();

	free(global_data.dest_host);

	return 0;
}
