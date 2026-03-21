#ifndef VERTEX
#define VERTEX

#include "Vector2D.h"

typedef struct _vertex {
	float x, y;
        Vector2D vel;
        int id;
} *Vertex;

Vertex Vertex_Construct(int id);

#endif
