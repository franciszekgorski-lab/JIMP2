#include "graph.h"
#include "vertex.h"

#include <stdio.h>
#include <stdlib.h>

Graph Graph_Construct() {
        Graph t = malloc(sizeof(struct _graph));
        t->Vertices = NULL;
        t->Edges = NULL;
        t->v_count = 0;
        t->e_count = 0;

        return t;
}

Vertex Graph_AddVertex(Graph g, int id) {
        if (g->Vertices == NULL) {
                g->Vertices = (Vertex*)malloc(sizeof(Vertex));
        } else {
                g->Vertices = (Vertex*)realloc(g->Vertices, (g->v_count + 1) * sizeof(Vertex));
        }

        g->Vertices[g->v_count] = Vertex_Construct(id);

        g->v_count++;

        return g->Vertices[g->v_count - 1];
}

void Graph_AddEdge(Graph g, int id, int idA, int idB, float w) {
        if (g->Edges == NULL) {
                g->Edges = (Edge*)malloc(sizeof(Edge));
        } else {
                g->Edges = (Edge*)realloc(g->Edges, (g->e_count + 1) * sizeof(Edge));
        }

        g->Edges[g->e_count] = Edge_Construct(id, w);

        int A_index = -1, B_index = -1;
        
        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == idA) A_index = i;
                if (g->Vertices[i]->id == idB) B_index = i;
        }

        if (A_index != -1) g->Edges[g->e_count]->A = g->Vertices[A_index];
        else g->Edges[g->e_count]->A = Graph_AddVertex(g, idA);
        
        if (B_index != -1) g->Edges[g->e_count]->B = g->Vertices[B_index];
        else g->Edges[g->e_count]->B = Graph_AddVertex(g, idB);

        g->e_count++;
}

int Graph_LoadFromFile(Graph g, const char* filepath) {
        FILE* file = fopen(filepath, "r");

        if (file == NULL) return;

        int id, idA, idB; float w;

        while ( fscanf(file, "%d %d %d %f", &id, &idA, &idB, &w) == 4 ) {
                Graph_AddEdge(g, id, idA, idB, w);
        }

        fclose(file);
}

void Graph_Print(Graph g) {
        printf("______________________EDGES_______________________\n");
        printf("AMOUNT: %d\n", g->e_count);
        for (int i = 0; i < g->e_count; i++) {
                Edge e = g->Edges[i];
                printf("%3d. %3d -> %3d : WEIGHT=%2.1f\n", e->id, e->A->id, e->B->id, e->weight);
        }

        printf("_____________________VERTICES_____________________\n");
        printf("AMOUNT: %d\n", g->v_count);
        for (int i = 0; i < g->v_count; i++) {
                Vertex v = g->Vertices[i];
                printf("%3d. (X, Y) = (%2.1f, %2.1f)\n", v->id, v->x, v->y);
        }
}
