#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "graph.h"
#include "Tutte.h"

int main(int argc, char* argv[]) {

	int opt;
        char* input_file;
        char* output_file;
		char flags = 0b00000000;
	
        Graph g = Graph_Construct();

	while ( (opt = getopt(argc, argv, "f:i:o:pa:v")) != -1 ) {
		switch (opt) {

		case 'i':
			flags |= 0b00000001;
                        input_file = optarg;
			break;
		
		case 'o':
			flags |= 0b00000010;
                        output_file = optarg;
			break;
		
		case 'p':
			flags |= 0b00000100;
			break;

		case 'f':
			if (strcmp(optarg, "txt") == 0) flags |= 0b00001000;
			else if (strcmp(optarg, "bin") != 0) {
				printf("Format must be txt or bin.");
			}
			break;

		case 'a':
			if (strcmp(optarg, "fruchterman") == 0) {
				flags |= 0b00010000;
			} else if (strcmp(optarg, "tutte") != 0) {
				printf("The program defaults to the Fruchterman-Reingold algorithm. To use the Tutte algorithm, specify the 'tutte' argument preceded by the -a flag.");
			}
			break;

                case 'v':
                        flags |= 0b00100000;
                        break;

		case '?':
	               	//invalid opt
			printf("[ERROR] ; Unknown option\n");
			break;
		}
	}

	if ( (flags&0b00000001) == 0b00000001) {
                Wczytywanie(g, input_file);
	}

	if ( (flags&0b00010000) == 0b00010000) {
		fruchterman_reingold(g, 500, time(NULL));
	} else {
		prototype(g, 1000000, 0);
	}

	if ( (flags&0b00000010) == 0b00000010) {
                Zapisywanie(g, output_file, flags);
	}

	if ( (flags&0b00000100) == 0b00000100) {
		Graph_Print(g);
	}

	return 0;
}
