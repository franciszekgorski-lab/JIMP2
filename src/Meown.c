#include "Meown.h"

void VertexForce(Graph g, int id) {
        Vertex v0 = NULL;
        Vertex v = NULL;
        Vector2D vel = NULL;

        for (int i = 0; i < g->v_count; i++) {
                Vertex v = g->Vertices[i];
                if (v->id == id) v0 = v;
        }

        for (int i = 0; i < g->e_count; i++) {
                Edge e = g->Edges[i];
                v = NULL;

                if (e->A->id == id) v = e->B;
                else if (e->B->id == id) v = e->A; 

                float max_length = DEFAULT_EDGE_LENGTH / e->weight;
                float distance = Graph_GetDistance(v0, v);                

                vel = Graph_GetForce(v0, v, 1);

                vel.x *= (max_length / distance) * RESISTANCE_FACTOR;
                vel.y *= (max_length / distance) * RESISTANCE_FACTOR;
                
                v0->vel.x += vel.x;
                v0->vel.y += vel.y;
        }

        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == id) continue;
                
                v = g->Vertices[i];
                
                vel = Graph_GetForce(v0, v, 0);

                v0->vel.x += vel.x;
                v0->vel.y += vel.y;
        }
}

void ForceFieldSim(Graph g, int stationary_id, int max_steps) {
        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == stationary_id) {
                        g->Vertices[i]->x = 0.0;
                        g->Vertices[i]->u = 0.0;
                        break;
                }
        }

        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertex[i]->id == stationary_id) continue;

                Vertex v = g->Vertices[i];
                v->x = i;
                v->y = 1;
        }

        for (int i = 0; i < max_steps; i++) {
                for (int j = 0; j < g->v_count; j++) {
                        if (g->Vertex[j]->id == stationary_id) continue;
                        VertexForce(g, g->Vertex[j]->id);
                        Vertex_Move(g->Vertex[j]);
                }
        }
}
