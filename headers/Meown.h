#ifndef MYOWN
#define MYWON

#define DEFAULT_EDGE_LENGTH 10
#define RESISTANCE_FACTOR 1.1
#define GENERAL_FORCE 0.5

#include "graph.h"

Vector2D VertexForce(Graph g, int id);

void ForceFieldSim(Graph g);

#endif
