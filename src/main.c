#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "Tutte.h"
#include "graph.h"

#define INPUT           0b00000001
#define OUTPUT          0b00000010
#define PRINT           0b00000100
#define FORMAT          0b00001000
#define ALGORYTHM       0b00010000
#define VISUALIZE       0b00100000

// main ogarnia parsowanie argumentow z linii polecen
// flagi sa trzymane jako bitmaska w jednym charze, kazdy bit to inny tryb
// -i input file, -o output file, -p print, -f format (txt/bin), -a algorytm, -v visualizer output
int main(int argc, char* argv[]) {
        
        int ret_mask = 0b00000000; // bitmaska zwracająca błędy

	int opt;
        char* input_file;
        char* output_file;
	char flags = 0b00000000; // bitmaska: bit0=input, bit1=output, bit2=print, bit3=txt, bit4=fruchterman/inspirowany tutte, bit5=vis
	
        Graph g = Graph_Construct();

	// getopt leci przez argv i zbiera opcje
	while ( (opt = getopt(argc, argv, "f:i:o:pa:v")) != -1 ) {
		switch (opt) {

		case 'i':
			flags |= INPUT; // nazwa pliku wejsciowego
                        input_file = optarg;
			break;
		
		case 'o':
			flags |= OUTPUT; // nazwa pliku wyjsciowego
                        output_file = optarg;
			break;
		
		case 'p': // drukuje graf na stdout
			flags |= PRINT; // drukuj do konsoli
			break;

		case 'f': // format zapisu - txt albo bin (domyslnie bin)
			if (strcmp(optarg, "txt") == 0) flags |= FORMAT;
			else if (strcmp(optarg, "bin") != 0) {
				printf("[ERROR] : Format must be txt or bin.");
                                ret_mask |= 0b1000;
			}
			break;

		case 'a': // wybor algorytmu algorytmu
			if (strcmp(optarg, "fruchterman") == 0) {
				flags |= ALGORYTHM;
			} else if (strcmp(optarg, "tutte") != 0) {
				printf("[ERROR] : Incorrect algorythm.\n");
                                ret_mask |= 0b10000;
			}
			break;

                case 'v': // tryb wyjscia dla visualizera (inny format pliku)
                        flags |= VISUALIZE;
                        break;

		case '?':
			printf("[ERROR] ; Unknown option\n");
                        ret_mask |= 0b100000;
			break;
		}
	}

	// wczytaj graf z pliku jesli podano -i
	if ( (flags&INPUT) == INPUT) {
                if (Wczytywanie(g, input_file)) ret_mask |= 0b1000000;
	} else {
                ret_mask |= 0b1;
        }

	// odpal algorytm - fruchterman jesli -a fruchterman, wpp prototype (tutte)
	if ( (flags&ALGORYTHM) == ALGORYTHM) {
		fruchterman_reingold(g, 500, time(NULL));
	} else {
		Physics(g, 1000);
                ret_mask |= 0b10;
	}

	// zapisz wynik do pliku jesli podano -o
	if ( (flags&OUTPUT) == OUTPUT) {
                Zapisywanie(g, output_file, flags);
	} else {
                ret_mask |= 0b100;
        }

	// wydrukuj graf na konsole jesli -p
	if ( (flags&PRINT) == PRINT) {
		Graph_Print(g);
	}

        // bit0=nie podano pliku do wczytania 
        // bit1=0 jesli wybrano fruchtermana, 1 jesli drugi
        // bit2=nie podano pliku do zapisu
        // bit3=niepoprawny format zapisu pliku
        // bit4=nie podano pliku do zapisu
        // bit5=nieznana flaga
        // bit6=nie znaleziono pliku wejsciowego

	return ret_mask;
}
