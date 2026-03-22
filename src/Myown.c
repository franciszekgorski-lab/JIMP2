#include "Myown.h"

#include <stddef.h>
#include <math.h>
#include <stdio.h>

void VertexForce(Graph g, Vertex v0) {
        Vertex v = NULL;
        Vector2D vel = Vector2D_Construct(0, 0);
        float distance = 0;

        for (int i = 0; i < g->e_count; i++) {
                Edge e = g->Edges[i];
                v = NULL;

                if (e->A->id == v0->id) v = e->B;
                else if (e->B->id == v0->id) v = e->A; 
                else continue;

                float max_length = DEFAULT_EDGE_LENGTH / e->weight;
                distance = Graph_GetDistance(v0, v);                

                if (distance < 0.0001f) continue;
                float stretch = distance - max_length; 

                vel = Graph_GetForce(v0, v, stretch > 0 ? 1 : 0);

                float magnitude = fabsf(stretch) * RESISTANCE_FACTOR;
                vel.x *= magnitude;
                vel.y *= magnitude;
                
                v0->vel.x += vel.x;
                v0->vel.y += vel.y;
        }

        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == v0->id) continue;
                
                v = g->Vertices[i];
                
                vel = Graph_GetForce(v0, v, 0);

                distance = Graph_GetDistance(v0, v);
                if (distance < 0.0001f) continue;

                if (distance > 0) {
                        float falloff = 5.0f / (distance * distance);
                        v0->vel.x += vel.x * falloff;
                        v0->vel.y += vel.y * falloff;
                }
        }

        Vertex virtual_center = Graph_CalculateCenter(g);
        
        vel = Graph_GetForce(v0, virtual_center, 0);
        distance = Graph_GetDistance(v0, virtual_center);

        vel.x *= 40 / distance;
        vel.y *= 40 / distance;

        v0->vel.x += vel.x;
        v0->vel.y += vel.y;
}

void ForceFieldSim(Graph g, int stationary_id, int max_steps) {
        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == stationary_id) {
                        g->Vertices[i]->x = 0.0;
                        g->Vertices[i]->y = 0.0;
                        break;
                }
        }

        for (int i = 0; i < g->v_count; i++) {
                if (g->Vertices[i]->id == stationary_id) continue;
                Vertex v = g->Vertices[i];
                float angle = (2.0f * M_PI * i) / g->v_count;
                v->x = 5.0f * cosf(angle);
                v->y = 5.0f * sinf(angle);
        }

        int i;
        int moving = 0;

        for (i = 0; i < max_steps; i++) {
                for (int j = 0; j < g->v_count; j++) {
                        Vertex v = g->Vertices[j];

                        if (v->id == stationary_id) continue;
                        VertexForce(g, v);
                                
                        if (v->vel.x < 0.0001f && v->vel.y < 0.0001f) moving++;
                                
                        Vertex_Move(v);
                }

                if (moving == g->v_count) break;
        }

        printf("generations:%d\n", i);
}
