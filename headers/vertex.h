#ifndef VERTEX
#define VERTEX

typedef struct _vertex {
	float x, y;
} *Vertex;

Vertex Vertex_Construct(float tx, float ty);

#endif
