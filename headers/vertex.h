#ifndef VERTEX
#define VERTEX

#include "Vector2D.h"

typedef struct _vertex {
	float x, y;
        Vector2D vel;
	Vector2D force;
        int id;
} *Vertex;

Vertex Vertex_Construct(int id);
void Vertex_Move(Vertex v);
void Vertex_ApplyForce(Vertex v);

#endif
