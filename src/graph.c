#include "graph.h"
#include "vertex.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

int Wczytywanie(Graph g, const char* filepath) {
        FILE* file = fopen(filepath, "r");

        if (file == NULL) return 1;

        int id, idA, idB; float w;

        while ( fscanf(file, "%d %d %d %f", &id, &idA, &idB, &w) == 4 ) {
                Graph_AddEdge(g, id, idA, idB, w);
        }

        fclose(file);
        return 0;
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

int Index(Graph g, Vertex v) {
        int i;
        for (i = 0; i < g->v_count; i++) {
                if (v == g->Vertices[i]) return i;
        }

        return -1;
}

void Zapisywanie(Graph g, const char* filepath, char flags) {
	if ( (flags&0b00100000) != 0b00100000) {
                if ((flags&0b00001000) == 0b00001000) {
		        FILE* file = fopen(filepath, "w");

		        if (file == NULL) return;

		        for (int i = 0; i < g->v_count; i++) {
			        Vertex v = g->Vertices[i];
			        fprintf(file, "%d %f %f\n", v->id, v->x, v->y);
		        }

		        fclose(file);
                } else if ((flags&0b00001000) != 0b00001000){
		        FILE* file = fopen(filepath, "wb");
		
		        if (file == NULL) return;

		        fwrite(g->Vertices, sizeof(Vertex), g->v_count, file);

		        fclose(file);
	        }
        } else {
                FILE* file = fopen(filepath, "w");

                if (file == NULL) return;

                fprintf(file, "%d %d\n", g->v_count, g->e_count);

                for (int i = 0; i < g->v_count; i++) {
                        Vertex v = g->Vertices[i];
                        fprintf(file, "%f %f\n", v->x, v->y);
                }

                for (int i = 0; i < g->e_count; i++) {
                        Edge e = g->Edges[i];
                        fprintf(file, "%d %d\n", Index(g, e->A), Index(g, e->B));
                }

                fclose(file);
        }
}
