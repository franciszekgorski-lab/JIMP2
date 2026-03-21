#include <stdio.h>
#include <unistd.h>

#include "graph.h"

int main(int argc, char* argv[]) {

	int opt;
        char* filepath;
	char loadflag = 0;
        Graph g = Graph_Construct();

	while ( (opt = getopt(argc, argv, "hi:")) != -1 ) {
		switch (opt) {

		case 'i':
			loadflag = 1;
                        filepath = optarg;
			break;

		case 'h':
			//usage description
			printf("[HELP] ; Existing options: -i -h \n");
			printf(" -i 'file_path', loads file \n");
			printf(" -h , help \n");
			break;

		case '?':
	               	//invalid opt
			printf("[ERROR] ; Unknown option\n");
			break;
		}
	}

	if (loadflag) {
                Graph_LoadFromFile(g, filepath);
                Graph_Print(g);   
	}

	return 0;
}
