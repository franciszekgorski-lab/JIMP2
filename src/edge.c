#include "edge.h"
#include "vertex.h"

#include <stdlib.h>

Edge Edge_Construct(int* id_assign) {
	Edge t = malloc(sizeof(Edge));

	t->A = malloc(sizeof(Vertex));
	t->B = malloc(sizeof(Vertex));
	t->weight = 0;
	t->id = (*id_assign)++;

	return t;
}
