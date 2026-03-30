#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../headers/graph.h"

void fruchterman_reingold(Graph g, int iterations, int seed) {
    if (g->v_count == 0 || g->e_count == 0) return;

    srand(seed);

    int n = g->v_count;
    int m = g->e_count;

    // inicjalizacja losowych pozycji
    for (int i = 0; i < n; i++) {
        g->Vertices[i]->x = (double)rand() / RAND_MAX;
        g->Vertices[i]->y = (double)rand() / RAND_MAX;
        g->Vertices[i]->vel.x = 0.0f;
        g->Vertices[i]->vel.y = 0.0f;
    }

    double area = 10.0;
    double k = sqrt(area / n);
    double temperature = 0.1;

    // tablica wektorów sił dla każdego wierzchołka
    Vector2D *disp = (Vector2D*)malloc(sizeof(Vector2D) * n);

    for (int iter = 0; iter < iterations; iter++) {
        // zerowanie sił
        for (int i = 0; i < n; i++) {
            disp[i].x = 0.0;
            disp[i].y = 0.0;
        }

        // SIŁY ODPYCHANIA
        for (int v = 0; v < n; v++) {
            for (int u = 0; u < n; u++) {
                if (v != u) {
                    double dx = g->Vertices[v]->x - g->Vertices[u]->x;
                    double dy = g->Vertices[v]->y - g->Vertices[u]->y;
                    double dist = sqrt(dx*dx + dy*dy) + 1e-9;

                    double force = (k * k) / dist;

                    disp[v].x += dx / dist * force;
                    disp[v].y += dy / dist * force;
                }
            }
        }

        // SIŁY PRZYCIĄGANIA
        for (int e = 0; e < m; e++) {
            Vertex A = g->Edges[e]->A;
            Vertex B = g->Edges[e]->B;

            double dx = A->x - B->x;
            double dy = A->y - B->y;
            double dist = sqrt(dx*dx + dy*dy) + 1e-9;

            double force = (dist * dist) / k * g->Edges[e]->weight;

            disp[A->id - 1].x -= dx / dist * force;
            disp[A->id - 1].y -= dy / dist * force;
            disp[B->id - 1].x += dx / dist * force;
            disp[B->id - 1].y += dy / dist * force;
        }

        // aktualizacja pozycji
        for (int i = 0; i < n; i++) {
            double disp_len = sqrt(disp[i].x*disp[i].x + disp[i].y*disp[i].y);
            if (disp_len > 0) {
                double move = fmin(disp_len, temperature);
                g->Vertices[i]->x += disp[i].x / disp_len * move;
                g->Vertices[i]->y += disp[i].y / disp_len * move;
            }
        }

        temperature *= 0.99;
    }

    // normalizacja współrzędnych do [0,1]
    double minx = g->Vertices[0]->x, maxx = g->Vertices[0]->x;
    double miny = g->Vertices[0]->y, maxy = g->Vertices[0]->y;

    for (int i = 0; i < n; i++) {
        if (g->Vertices[i]->x < minx) minx = g->Vertices[i]->x;
        if (g->Vertices[i]->x > maxx) maxx = g->Vertices[i]->x;
        if (g->Vertices[i]->y < miny) miny = g->Vertices[i]->y;
        if (g->Vertices[i]->y > maxy) maxy = g->Vertices[i]->y;
    }

    for (int i = 0; i < n; i++) {
        g->Vertices[i]->x = (g->Vertices[i]->x - minx) / (maxx - minx + 1e-9);
        g->Vertices[i]->y = (g->Vertices[i]->y - miny) / (maxy - miny + 1e-9);
    }

    free(disp);
}