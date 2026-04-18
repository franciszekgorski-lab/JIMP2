#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../headers/graph.h"

// implementacja algorytmu Fruchtermana-Reingolda do layoutu grafu
// dziala na zasadzie symulacji sil - wierzcholki sie odpychaja, krawedzie przyciagaja
// iterations = ile krokow symulacji, seed = ziarno dla rand (powtarzalnosc)
void fruchterman_reingold(Graph g, int iterations, int seed) {
    if (g->v_count == 0 || g->e_count == 0) return;

    srand(seed);

    int n = g->v_count;
    int m = g->e_count;

    // losowe startowe pozycje w [0,1]
    for (int i = 0; i < n; i++) {
        g->Vertices[i]->x = (double)rand() / RAND_MAX;
        g->Vertices[i]->y = (double)rand() / RAND_MAX;
        g->Vertices[i]->vel.x = 0.0f;
        g->Vertices[i]->vel.y = 0.0f;
    }

    double area = 10.0;
    double k = sqrt(area / n); // optymalna odleglosc miedzy wierzcholkami
    double temperature = 0.1;  // ogranicza max przesuniecie w jednym kroku, maleje z czasem

    // tymczasowa tablica przesuniec dla kazdego wierzcholka
    Vector2D *disp = (Vector2D*)malloc(sizeof(Vector2D) * n);

    for (int iter = 0; iter < iterations; iter++) {
        // zeruj przesuniec przed kazdym krokiem
        for (int i = 0; i < n; i++) {
            disp[i].x = 0.0;
            disp[i].y = 0.0;
        }

        // sily odpychania - kazda para wierzcholkow sie odpycha (O(n^2))
        // sila = k^2 / dist
        for (int v = 0; v < n; v++) {
            for (int u = 0; u < n; u++) {
                if (v != u) {
                    double dx = g->Vertices[v]->x - g->Vertices[u]->x;
                    double dy = g->Vertices[v]->y - g->Vertices[u]->y;
                    double dist = sqrt(dx*dx + dy*dy) + 1e-9; // +epsilon zeby uniknac dzielenia przez 0

                    double force = (k * k) / dist;

                    disp[v].x += dx / dist * force;
                    disp[v].y += dy / dist * force;
                }
            }
        }

        // sily przyciagania wzdluz krawedzi
        // sila = dist^2 / k, skalowana przez wage krawedzi
        for (int e = 0; e < m; e++) {
            Vertex A = g->Edges[e]->A;
            Vertex B = g->Edges[e]->B;

            double dx = A->x - B->x;
            double dy = A->y - B->y;
            double dist = sqrt(dx*dx + dy*dy) + 1e-9;

            double force = (dist * dist) / k * g->Edges[e]->weight;

            // A przyciagane do B, B do A
            disp[A->id - 1].x -= dx / dist * force;
            disp[A->id - 1].y -= dy / dist * force;
            disp[B->id - 1].x += dx / dist * force;
            disp[B->id - 1].y += dy / dist * force;
        }

        // aktualizuj pozycje, ale max przesuniecie ograniczone temperatura
        for (int i = 0; i < n; i++) {
            double disp_len = sqrt(disp[i].x*disp[i].x + disp[i].y*disp[i].y);
            if (disp_len > 0) {
                double move = fmin(disp_len, temperature);
                g->Vertices[i]->x += disp[i].x / disp_len * move;
                g->Vertices[i]->y += disp[i].y / disp_len * move;
            }
        }

        temperature *= 0.99; // chlodzenie - im pozniej tym mniejsze ruchy
    }

    // normalizacja do [0,1] po zakonczeniu symulacji
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
