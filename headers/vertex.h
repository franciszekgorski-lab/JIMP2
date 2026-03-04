#ifndef VERTEX
#define VERTEX

typedef struct _vertex {
	float x, y;
	int id;
} *Vertex;

Vertex Vertex_Construct(int* id_assign);

#endif
