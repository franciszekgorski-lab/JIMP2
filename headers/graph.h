#ifndef GRAPH
#define GRAPH

#include "vertex.h"
#include "edge.h"

typedef struct _graph {
	Vertex* Vertices;	// vertex list
	Edge* Edges;		// edge list
	int e_count;		// edge count
	int v_count;		// vertex count
} *Graph;

Graph Graph_Construct();        // te - temporary edge num
			        // tv - temporary vertex count
Vertex Graph_AddVertex(Graph g, int id);
void Graph_AddEdge(Graph g, int id, int idA, int idB, float w);
int Graph_LoadFromFile(Graph g, const char* filepath);
void Graph_Print(Graph g);
Vector2D Graph_GetForce(Vertex v0, Vertex v, char option);
float Graph_GetDistance(Vertex v0, Vertex v);
Vertex Graph_CalculateCenter(Graph g);

#endif
