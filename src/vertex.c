#include "vertex.h"

#include <stdlib.h>

Vertex Vertex_Construct(int* id_assign) {
	Vertex t = malloc(sizeof(Vertex));
	t->x = 0;
	t->y = 0;
	t->id = (*id_assign)++;

	return t;
}

