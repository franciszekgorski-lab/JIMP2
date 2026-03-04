#ifndef EDGE
#define EDGE

#include "vertex.h"

typedef struct _edge {
	Vertex VertexA;
	Vertex VertexB;
	float weight;
	char* name;
} *Edge;

Edge Edge_Construct(Vertex A, Vertex B, float w, const char* n);

#endif
