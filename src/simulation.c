#include "graph.h"

#include <math.h>
#include <stdio.h>

#define k 0.1
#define damping 0.7
#define MIN_VEL 0.001
#define MAX_VEL 10
#define RADIUS 5
#define MAX_EDGE_LEN 8.0f

Vector2D dist(Vertex v0, Vertex v1, float* distance) {
        float dx = v1->x - v0->x;
        float dy = v1->y - v0->y;
        
        Vector2D d;
        d.x = dx;
        d.y = dy;

        *distance = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

        return d;
}
void apply_constraints(Graph g) {
        for (int i = 0; i < g->e_count; i++) {
                Vertex A = g->Edges[i]->A;
                Vertex B = g->Edges[i]->B;

                float dx = B->x - A->x;
                float dy = B->y - A->y;
                float dist = sqrt(dx*dx + dy*dy);

                if (dist > MAX_EDGE_LEN) {
                        // korekcja pozycji
                        float correction = (dist - MAX_EDGE_LEN * g->Edges[i]->weight) / dist * 0.5f;
                        A->x += dx * correction;
                        A->y += dy * correction;
                        B->x -= dx * correction;
                        B->y -= dy * correction;

                        // znormalizowany wektor wzdłuż krawędzi
                        float nx = dx / dist;
                        float ny = dy / dist;

                        // składowa prędkości A wzdłuż krawędzi (iloczyn skalarny)
                        float dot_A = A->vel.x * nx + A->vel.y * ny;
                        // składowa prędkości B wzdłuż krawędzi
                        float dot_B = B->vel.x * nx + B->vel.y * ny;

                        // odejmij tylko rozciągającą część —
                        // A "ciągnie" w kierunku +nx, B w kierunku -nx
                        if (dot_A > 0) {
                                A->vel.x -= dot_A * nx;
                                A->vel.y -= dot_A * ny;
                        }

                        if (dot_B < 0) {
                                B->vel.x -= dot_B * nx;
                                B->vel.y -= dot_B * ny;
                        }
                }
        }
}

void apply_forces(Graph g, Vertex v0) {
        Vector2D dvector;
        float distance;

        v0->force.x = 0;
        v0->force.y = 0;
        
        for (int i = 0; i < g->v_count; i++) {
                Vertex v1 = g->Vertices[i];
                
                if (v1->id == v0->id) continue;

                dvector = dist(v0, v1, &distance);

                dvector.x *= -1.0;
                dvector.y *= -1.0;

                dvector.x /= pow(distance, 2.0);
                dvector.y /= pow(distance, 2.0);

                dvector.x *= k;
                dvector.y *= k;

                v0->force.x += dvector.x;
                v0->force.y += dvector.y;
        }

        v0->vel.x += v0->force.x;
        v0->vel.y += v0->force.y;
}

int handle_damping(Vertex v0) {
        v0->vel.x *= damping;
        v0->vel.y *= damping;

        float velocity = sqrt(pow(v0->vel.x, 2.0) + pow(v0->vel.y, 2.0));

        if (velocity <= MIN_VEL) return 1;
        if (velocity >= MAX_VEL) {

        }

        return 0;
}

void Physics(Graph g, int max_i) {
        
        for (int i = 0; i < g->v_count; i++) {
                Vertex v = g->Vertices[i];

                float radians = 2.0 * M_PI * (1.0 / g->v_count);

                v->x = RADIUS * cos(i * radians);
                v->y = RADIUS * sin(i * radians);
                v->vel.x = MIN_VEL;
        }

        int i;
        for (i = 0; i < max_i; i++) {
                float total_energy = 0.0f;
    
                for (int j = 0; j < g->v_count; j++) {
                        Vertex v0 = g->Vertices[j];
                        handle_damping(v0);
                        apply_forces(g, v0);
                        Vertex_Move(v0);
                        total_energy += v0->vel.x * v0->vel.x + v0->vel.y * v0->vel.y;
                }
            
                apply_constraints(g);

                if (total_energy < MIN_VEL * MIN_VEL * g->v_count) break;
        }
        float dx = -(g->Vertices[0]->x);
        float dy = -(g->Vertices[0]->y);

        g->Vertices[0]->x = 0;
        g->Vertices[0]->y = 0;

        for (int j = 1; j < g->v_count; j++) {
                g->Vertices[j]->x += dx;
                g->Vertices[j]->y += dy;
        }

        printf("%d\n", i);
}
 
