#include "graph.h"
#include "vertex.h"
#include "Myown.h"

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

Vector2D Graph_GetForce(Vertex v0, Vertex v, char option) {
        float x = (v0->x - v->x) < 0 ? (v->x - v0->x) : (v0->x - v->x);
        float y = (v0->y - v->y) < 0 ? (v->y - v0->y) : (v0->y - v->y);

        float dims_ratio;

        if (y != 0) dims_ratio = x / y;
        else dims_ratio = 1;

        float x_vel = 0;
        float y_vel = 0;

        if (dims_ratio != 0) {
                if (dims_ratio < 1) {
                        x_vel = GENERAL_FORCE * dims_ratio;
                        y_vel = GENERAL_FORCE - x_vel;
                } else {
                        dims_ratio = 1 / dims_ratio;
                        y_vel = GENERAL_FORCE * dims_ratio;
                        x_vel = GENERAL_FORCE - y_vel;
                }
        } else {
                x_vel = 0;
                y_vel = GENERAL_FORCE * sqrt(2);
        }

        if (option == 0) {
                if (v0->x < v->x) x_vel *= -1;
                if (v0->y < v->y) y_vel *= -1;
        } else {
                if (v0->x > v->x) x_vel *= -1;
                if (v0->y > v->y) y_vel *= -1;
        }

        Vector2D vel = Vector2D_Construct(x_vel, y_vel);

        return vel;
}

float Graph_GetDistance(Vertex v0, Vertex v) {
        float x = (v0->x - v->x) < 0 ? (v->x - v0->x) : (v0->x - v->x);
        float y = (v0->y - v->y) < 0 ? (v->y - v0->y) : (v0->y - v->y);
        
        float distance = sqrt(x*x + y*y);

        return distance;
}

Vertex Graph_CalculateCenter(Graph g) {
        Vertex center = Vertex_Construct(0);

        float gen_x = 0;
        float gen_y = 0;

        for (int i = 0; i < g->v_count; i++) gen_x += g->Vertices[i]->x;
        for (int i = 0; i < g->v_count; i++) gen_y += g->Vertices[i]->y;

        gen_x /= g->v_count;
        gen_y /= g->v_count;

        center->x = gen_x;
        center->y = gen_y;

        return center;
}
