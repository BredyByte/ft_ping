#ifndef UTILS_H
# define UTILS_H

int		hex_serializer(void);
void	print_help(void);
void	print_usage(void);
void	print_version(void);
void	print_args(void);
void	exit_failure(const char *message);
void 	free_allocations();
void	sleep_microseconds(long microseconds);

#endif
