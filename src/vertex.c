#include "vertex.h"

#include <stdlib.h>

Vertex Vertex_Construct(int id) {
	Vertex t = malloc(sizeof(struct _vertex));
	t->x = 0;
	t->y = 0;
        t->id = id;
        t->vel = Vector2D_Construct(0.0f, 0.0f);

	return t;
}

void Vertex_Move(Vertex v) {
        v->x += v->vel.x;
        v->y += v->vel.y;
}
