#include <stdio.h>
#include <unistd.h>

#include "graph.h"
#include "Tutte.h"

int main(int argc, char* argv[]) {

	int opt;
        char* input_file;
        char* output_file;
	char flags = 0b0000;
        Graph g = Graph_Construct();

	while ( (opt = getopt(argc, argv, "i:o:p")) != -1 ) {
		switch (opt) {

		case 'i':
			flags |= 0b0001;
                        input_file = optarg;
			break;
		
		case 'o':
			flags |= 0b0010;
                        output_file = optarg;
			break;
		
		case 'p':
			flags |= 0b0100;
			break;

		case '?':
	               	//invalid opt
			printf("[ERROR] ; Unknown option\n");
			break;
		}
	}

	if ( (flags&0b0001) == 0b0001) {
                Wczytywanie(g, input_file);
                ForceFieldSim(g, 1, 100000);
	}
	
	if ( (flags&0b0010) == 0b0010) {
                Zapisywanie(g, output_file);
	}

	if ( (flags&0b0100) == 0b0100) {
		Graph_Print(g);
	}

	return 0;
}
