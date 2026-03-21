#include "vertex.h"

#include <stdlib.h>

Vertex Vertex_Construct(int id) {
	Vertex t = malloc(sizeof(struct _vertex));
	t->x = 0;
	t->y = 0;
        t->id = id;

	return t;
}

