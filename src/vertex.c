#include "vertex.h"

#include <stdlib.h>

// alokuje wierzcholek, pozycja (0,0), zerowa predkosc
Vertex Vertex_Construct(int id) {
	Vertex t = malloc(sizeof(struct _vertex));
	t->x = 0;
	t->y = 0;
        t->id = id;
        t->vel = Vector2D_Construct(0.0f, 0.0f);

	return t;
}

// przesuwa wierzcholek o jego aktualny wektor predkosci
void Vertex_Move(Vertex v) {
        v->x += v->vel.x;
        v->y += v->vel.y;
}

// dodaje sile do predkosci (akumulacja sil)
void Vertex_ApplyForce(Vertex v) {
	v->vel.x += v->force.x;
	v->vel.y += v->force.y;
}
