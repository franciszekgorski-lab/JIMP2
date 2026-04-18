#include "edge.h"
#include "vertex.h"

#include <stdlib.h>

// tworzy krawedz z id i waga, wierzcholki ustawiane pozniej przez Graph_AddEdge
Edge Edge_Construct(int id, float w) {
	Edge t = malloc(sizeof(struct _edge));

	t->A = NULL;
	t->B = NULL;
	t->weight = w;
        t->id = id;

	return t;
}
