# include "defines.h"
# include <math.h>
# include <stdio.h>

double  calc_max_rtt(const double arr[], int size)
{
    if (arr == NULL || size <= 0)
        return 0;

    double  max = arr[0];

    for (int i = 1; i < size; i++)
        max = (arr[i] > max) ? arr[i] : max;

    return max;
}

double  calc_min_rtt(const double arr[], int size)
{
    if (arr == NULL || size <= 0)
        return 0;

    double  min = arr[0];

    for (int i = 1; i < size; i++)
        min = (arr[i] < min) ? arr[i] : min;

    return min;
}

double  calc_avg_rrt(const double arr[], int size)
{
    if (arr == NULL || size <= 0)
        return 0;

    double  sum = arr[0];

    for (int i = 1; i < size; i++)
        sum += arr[i];

    return sum / size;
}

double  calc_stddev_rrt(const double arr[], int size)
{
    if (arr == NULL || size <= 0)
        return 0;

    double  diffs_avg[size];
    double  aux = 0;


    // Calculate squared differences between each RTT and the average RTT
    for (int i = 0; i < size; i++)
    {
        aux = arr[i] - g_data.stats.rtt_avg;
        diffs_avg[i] = aux * aux;
    }

    // Sum the squared differences
    aux = 0;
    for (int i = 0; i < size; i++)
        aux += diffs_avg[i];


    return (aux / size);
}

void    print_stats()
{
	int packets_lost = g_data.stats.packets_transmitted - g_data.stats.packets_received;
	int packet_loss_percent = (packets_lost > 0) ?
								(packets_lost * 100) / g_data.stats.packets_transmitted : 0;

	// Calculate statistics for RTT values
    g_data.stats.rtt_min = calc_min_rtt(g_data.stats.rtt_values, g_data.stats.rtt_count);
    g_data.stats.rtt_max = calc_max_rtt(g_data.stats.rtt_values, g_data.stats.rtt_count);
    g_data.stats.rtt_avg = calc_avg_rrt(g_data.stats.rtt_values, g_data.stats.rtt_count);
    g_data.stats.rtt_stddev = calc_stddev_rrt(g_data.stats.rtt_values, g_data.stats.rtt_count);

    printf("--- %s ping statistics ---\n", g_data.dest_host);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
           g_data.stats.packets_transmitted, g_data.stats.packets_received, packet_loss_percent);
    if (g_data.stats.packets_received > 0)
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
               g_data.stats.rtt_min, g_data.stats.rtt_avg, g_data.stats.rtt_max,
                g_data.stats.rtt_stddev);
}
