#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

	int opt;
	char loadflag = 0;

	while ( (opt = getopt(argc, argv, "hf:")) != -1 ) {
		switch (opt) {

		case 'f':
			loadflag = 1;
			break;

		case 'h':
			//usage description
			printf("[HELP] ; Existing options: -f -h \n");
			printf(" -f 'file_path', loads file \n");
			printf(" -h , help \n");
			break;

		case '?':
			//invalid opt
			printf("[ERROR] ; Unknown option\n");
			break;
		}
	}

	if (loadflag) {
	
	}

	return 0;
}
