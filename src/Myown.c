#include "Myown.h"
#include "graph.h"
#include "vertex.h"
#include "Vector2D.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* -------------------------------------------------------
 * Tutte's Embedding
 *
 * Algorytm:
 *  1. Wybierz k wierzchołków granicznych i rozmieść je
 *     równomiernie na okręgu jednostkowym (fixed vertices).
 *  2. Dla każdego pozostałego wierzchołka (free) iteracyjnie
 *     ustawiaj jego pozycję jako średnią pozycji sąsiadów:
 *
 *        x_i = (1 / deg(i)) * sum_{j in N(i)} x_j
 *        y_i = (1 / deg(i)) * sum_{j in N(i)} y_j
 *
 *  3. Powtarzaj aż do zbieżności (max_iter razy lub eps).
 * ------------------------------------------------------- */

#define TUTTE_RADIUS   100.0f   /* promień okręgu dla wierzchołków granicznych */
#define TUTTE_EPSILON  1e-5f    /* próg zbieżności                             */

/* Zwraca liczbę sąsiadów wierzchołka v_id w grafie g
   oraz wypełnia tablicę neighbour_ids (musi mieć rozmiar >= g->e_count*2) */
static int get_neighbours(Graph g, int v_id,
                           int* neighbour_ids, int max_n) {
    int count = 0;
    for (int i = 0; i < g->e_count; i++) {
        Edge e = g->Edges[i];
        if (e->A->id == v_id && count < max_n) {
            neighbour_ids[count++] = e->B->id;
        } else if (e->B->id == v_id && count < max_n) {
            neighbour_ids[count++] = e->A->id;
        }
    }
    return count;
}

/* Zwraca indeks wierzchołka o danym id w tablicy g->Vertices, lub -1 */
static int find_vertex_index(Graph g, int id) {
    for (int i = 0; i < g->v_count; i++) {
        if (g->Vertices[i]->id == id) return i;
    }
    return -1;
}

void ForceFieldSim(Graph g, int boundary_count_hint, int max_iter) {
    if (g->v_count == 0) return;

    /* --- 1. Ustal liczbę wierzchołków granicznych ---
     * Bierzemy min(boundary_count_hint, v_count) pierwszych wierzchołków
     * jako fixed. Jeśli hint <= 0, bierzemy sqrt(v_count) (min 3). */
    int k = boundary_count_hint > 0
                ? boundary_count_hint
                : (int)sqrtf((float)g->v_count);
    if (k < 3)  k = 3;
    if (k > g->v_count) k = g->v_count;

    /* Tablica flag: fixed[i] = 1 gdy wierzchołek i jest graniczny */
    char* fixed = (char*)calloc(g->v_count, sizeof(char));
    if (!fixed) return;

    /* --- 2. Rozmieść wierzchołki graniczne na okręgu --- */
    for (int i = 0; i < k; i++) {
        float angle = 2.0f * (float)M_PI * i / (float)k;
        g->Vertices[i]->x = TUTTE_RADIUS * cosf(angle);
        g->Vertices[i]->y = TUTTE_RADIUS * sinf(angle);
        fixed[i] = 1;
    }

    /* --- 3. Pozostałe wierzchołki startują w centrum --- */
    for (int i = k; i < g->v_count; i++) {
        g->Vertices[i]->x = 0.0f;
        g->Vertices[i]->y = 0.0f;
    }

    /* Bufor na indeksy sąsiadów */
    int* neighbours = (int*)malloc(g->e_count * 2 * sizeof(int));
    if (!neighbours) { free(fixed); return; }

    /* --- 4. Iteracje Tutte --- */
    for (int iter = 0; iter < max_iter; iter++) {
        float max_delta = 0.0f;

        for (int i = 0; i < g->v_count; i++) {
            if (fixed[i]) continue;   /* wierzchołki graniczne nie ruszamy */

            Vertex vi = g->Vertices[i];

            int n = get_neighbours(g, vi->id, neighbours,
                                   g->e_count * 2);
            if (n == 0) continue;     /* izolowany wierzchołek — zostaw */

            float sum_x = 0.0f, sum_y = 0.0f;
            for (int j = 0; j < n; j++) {
                int idx = find_vertex_index(g, neighbours[j]);
                if (idx == -1) continue;
                sum_x += g->Vertices[idx]->x;
                sum_y += g->Vertices[idx]->y;
            }

            float new_x = sum_x / (float)n;
            float new_y = sum_y / (float)n;

            float dx = new_x - vi->x;
            float dy = new_y - vi->y;
            float delta = sqrtf(dx*dx + dy*dy);
            if (delta > max_delta) max_delta = delta;

            vi->x = new_x;
            vi->y = new_y;
        }

        /* Zbieżność — przerywamy wcześniej jeśli ruch < epsilon */
        if (max_delta < TUTTE_EPSILON) {
            printf("[Tutte] Converged after %d iterations (delta=%.2e)\n",
                   iter + 1, (double)max_delta);
            break;
        }
    }

    free(neighbours);
    free(fixed);
}
