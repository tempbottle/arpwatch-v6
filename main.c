#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "utils.h"
#include "config.h"
#include "capture.h"
#include "main.h"

int opt_debug = 0;
network_config_t networks[];
extern int num_networks;

int main(int argc, char *argv[]) {
	int c, i, j;
	char *config_file = NULL;
	char *base_filter = "(icmp6 or arp) and ";
	char *dev;

	/* read in command line options */
	while ((c = getopt(argc, argv, "c:d")) != -1) {
		switch(c) {
			case 'd':
				opt_debug = 1;
				printf("option -d\n");
				break;
			case 'c':
				config_file = optarg;
				if (opt_debug) {
					printf("option -c: %s\n", config_file);
				}
				break;
			default:
				usage();
				break;
		}
	}

	if (!config_read(config_file)) {
		fprintf(stderr, "Failed to read config file\n");
		die();
	}

	if (opt_debug) printf("Number of networks to monitor: %i\n", num_networks);
	for (i = 0; i < num_networks; i++) {
		/* build the filter based on the provided filter and base_filter */
		char *filter_expr = malloc(strlen(networks[i].filter) + strlen(base_filter) + 2);
		sprintf(filter_expr, "%s(%s)", base_filter, networks[i].filter);
		
		if (opt_debug) {
			printf("Network: %s (%i)\n", networks[i].name, i);
			printf("\tdevice = %s\n", networks[i].device);
			printf("\tfilter = %s\n", filter_expr);
			printf("\tDatabases:\n");
			for (j = 0; j < networks[i].num_databases; j++) {
				printf("\t\t%s\n", networks[i].databases[j]);
			}
		}
		dev = networks[i].device;
		capture(dev, filter_expr);
	}

	return 0;
}

