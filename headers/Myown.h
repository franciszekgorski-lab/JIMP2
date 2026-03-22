#ifndef MYOWN
#define MYWON

#define DEFAULT_EDGE_LENGTH 5
#define RESISTANCE_FACTOR 11
#define GENERAL_FORCE 0.1

#include "graph.h"

void VertexForce(Graph g, Vertex v0);

void ForceFieldSim(Graph g, int stationary_id, int max_steps);

#endif
