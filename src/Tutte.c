#include "Tutte.h"
#include "graph.h"
#include "vertex.h"
#include "Vector2D.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TUTTE_RADIUS    100.0f
#define TUTTE_EPSILON   1e-6f
#define TUTTE_MAX_ITER  10000

/* ---------------------------------------------------------------
 * Pomocnicze: stopień wierzchołka o indeksie v_idx
 * --------------------------------------------------------------- */
static int degree(Graph g, int v_idx) {
    int id  = g->Vertices[v_idx]->id;
    int cnt = 0;
    for (int i = 0; i < g->e_count; i++) {
        Edge e = g->Edges[i];
        if (e->A->id == id || e->B->id == id) cnt++;
    }
    return cnt;
}

/* ---------------------------------------------------------------
 * Pomocnicze: wypełnia nb[] indeksami sąsiadów v_idx.
 * Zwraca liczbę sąsiadów. nb musi mieć >= e_count miejsc.
 * --------------------------------------------------------------- */
static int neighbours(Graph g, int v_idx, int* nb) {
    int id  = g->Vertices[v_idx]->id;
    int cnt = 0;
    for (int i = 0; i < g->e_count; i++) {
        Edge e = g->Edges[i];
        int other_id = -1;
        if (e->A->id == id) other_id = e->B->id;
        else if (e->B->id == id) other_id = e->A->id;
        else continue;

        /* Szukaj indeksu sąsiada */
        for (int j = 0; j < g->v_count; j++) {
            if (g->Vertices[j]->id == other_id) {
                nb[cnt++] = j;
                break;
            }
        }
    }
    return cnt;
}

/* ---------------------------------------------------------------
 * BFS: zwraca tablicę visited[] — true dla wierzchołków
 * osiągalnych ze start_idx. Używane do wykrycia spójności.
 * --------------------------------------------------------------- */
static void bfs_reachable(Graph g, int start_idx, char* visited) {
    int* queue = (int*)malloc(g->v_count * sizeof(int));
    int* nb     = (int*)malloc(g->e_count * 2 * sizeof(int));
    if (!queue || !nb) { free(queue); free(nb); return; }

    int head = 0, tail = 0;
    visited[start_idx] = 1;
    queue[tail++] = start_idx;

    while (head < tail) {
        int cur = queue[head++];
        int n   = neighbours(g, cur, nb);
        for (int i = 0; i < n; i++) {
            if (!visited[nb[i]]) {
                visited[nb[i]] = 1;
                queue[tail++] = nb[i];
            }
        }
    }

    free(queue);
    free(nb);
}

/* ---------------------------------------------------------------
 * Znajdź cykl metodą BFS (nie DFS!) — szukamy najkrótszego cyklu
 * przechodzącego przez wierzchołek start_idx.
 * Zwraca długość cyklu lub 0. cycle_out[] = indeksy wierzchołków.
 *
 * Idea: BFS od start, gdy natrafimy na krawędź łączącą dwa węzły
 * z tego samego poziomu lub z sąsiednich poziomów — mamy cykl.
 * Odtwarzamy go przez tablicę parent[].
 * --------------------------------------------------------------- */
static int bfs_find_cycle(Graph g, int start_idx,
                           int* cycle_out) {
    int  n     = g->v_count;
    int* dist  = (int*)malloc(n * sizeof(int));
    int* par   = (int*)malloc(n * sizeof(int));
    int* queue = (int*)malloc(n * sizeof(int));
    int* nb    = (int*)malloc((g->e_count * 2 + 1) * sizeof(int));

    if (!dist || !par || !queue || !nb) {
        free(dist); free(par); free(queue); free(nb);
        return 0;
    }

    for (int i = 0; i < n; i++) { dist[i] = -1; par[i] = -1; }

    int head = 0, tail = 0;
    dist[start_idx] = 0;
    queue[tail++]   = start_idx;

    int u_cycle = -1, v_cycle = -1;

    while (head < tail && u_cycle == -1) {
        int cur  = queue[head++];
        int nn   = neighbours(g, cur, nb);

        for (int i = 0; i < nn && u_cycle == -1; i++) {
            int nbr = nb[i];
            if (nbr == par[cur]) continue; /* Nie wracamy do rodzica */

            if (dist[nbr] == -1) {
                dist[nbr] = dist[cur] + 1;
                par[nbr]  = cur;
                queue[tail++] = nbr;
            } else if (dist[nbr] >= dist[cur]) {
                /* Krawędź poprzeczna / wsteczna — mamy cykl */
                u_cycle = cur;
                v_cycle = nbr;
            }
        }
    }

    int cycle_len = 0;

    if (u_cycle != -1 && v_cycle != -1) {
        /* Odtwórz ścieżkę od start do u_cycle i od start do v_cycle,
           złącz je w cykl.  */
        int* path_u = (int*)malloc(n * sizeof(int));
        int* path_v = (int*)malloc(n * sizeof(int));
        int pu = 0, pv = 0;

        if (path_u && path_v) {
            /* ścieżka do u_cycle */
            for (int cur = u_cycle; cur != -1; cur = par[cur])
                path_u[pu++] = cur;
            /* ścieżka do v_cycle */
            for (int cur = v_cycle; cur != -1; cur = par[cur])
                path_v[pv++] = cur;

            /* Znajdź LCA (najniższego wspólnego przodka) — to gdzie ścieżki
               się rozchodzą. Ścieżki kończą się na start_idx. */
            int lca = start_idx;
            /* Porównaj od końca (od korzenia) */
            int i = pu - 1, j = pv - 1;
            while (i >= 0 && j >= 0 && path_u[i] == path_v[j]) {
                lca = path_u[i];
                i--; j--;
            }
            /* Cykl: path_u[0..i+1] + path_v[j+1..0], przez LCA */
            int len = (i + 2) + (j + 2);
            if (len >= 3 && len <= n) {
                for (int x = 0; x <= i + 1; x++)
                    cycle_out[cycle_len++] = path_u[x];
                for (int x = j + 1; x >= 0; x--)
                    cycle_out[cycle_len++] = path_v[x];
            }
        }

        free(path_u);
        free(path_v);
    }

    free(dist); free(par); free(queue); free(nb);
    return cycle_len;
}

/* ---------------------------------------------------------------
 * Znajdź zewnętrzny cykl grafu.
 * Strategia:
 *   1. Spróbuj BFS-cycle dla wierzchołka o największym stopniu.
 *   2. Jeśli brak cyklu (drzewo), użyj wszystkich wierzchołków.
 * Zwraca długość cyklu, wypełnia cycle_out[].
 * --------------------------------------------------------------- */
static int find_outer_cycle(Graph g, int* cycle_out) {
    if (g->v_count < 3) {
        for (int i = 0; i < g->v_count; i++) cycle_out[i] = i;
        return g->v_count;
    }

    /* Wierzchołek startowy = ten o największym stopniu */
    int start = 0, max_deg = 0;
    for (int i = 0; i < g->v_count; i++) {
        int d = degree(g, i);
        if (d > max_deg) { max_deg = d; start = i; }
    }

    int cycle_len = bfs_find_cycle(g, start, cycle_out);

    /* Fallback: próbuj od innych wierzchołków */
    if (cycle_len < 3) {
        for (int s = 0; s < g->v_count && cycle_len < 3; s++) {
            if (s == start) continue;
            cycle_len = bfs_find_cycle(g, s, cycle_out);
        }
    }

    /* Ostatni fallback: brak cyklu — wszystkie wierzchołki na okrąg */
    if (cycle_len < 3) {
        for (int i = 0; i < g->v_count; i++) cycle_out[i] = i;
        return g->v_count;
    }

    return cycle_len;
}

/* ---------------------------------------------------------------
 * Iteracje Gaussa-Seidela dla wewnętrznych wierzchołków.
 * Każdy swobodny wierzchołek = ważona średnia sąsiadów (wagi = wagi krawędzi).
 * --------------------------------------------------------------- */
static void gauss_seidel(Graph g, char* fixed, int* nb_buf, int max_iter) {
    for (int iter = 0; iter < max_iter; iter++) {
        float max_delta = 0.0f;

        for (int i = 0; i < g->v_count; i++) {
            if (fixed[i]) continue;

            /* Ważona średnia po sąsiadach (waga krawędzi) */
            int n = g->e_count * 2;
            int* nb = nb_buf; /* używamy wspólnego bufora */
            int cnt = neighbours(g, i, nb);
            if (cnt == 0) continue;

            /* Zbierz wagi krawędzi do sąsiadów */
            float sum_wx = 0.0f, sum_wy = 0.0f, sum_w = 0.0f;
            int id = g->Vertices[i]->id;

            for (int e = 0; e < g->e_count; e++) {
                Edge edge = g->Edges[e];
                int  other_id = -1;
                if      (edge->A->id == id) other_id = edge->B->id;
                else if (edge->B->id == id) other_id = edge->A->id;
                else continue;

                /* Znajdź indeks sąsiada */
                for (int j = 0; j < g->v_count; j++) {
                    if (g->Vertices[j]->id == other_id) {
                        float w = (edge->weight > 0.0f) ? edge->weight : 1.0f;
                        sum_wx += w * g->Vertices[j]->x;
                        sum_wy += w * g->Vertices[j]->y;
                        sum_w  += w;
                        break;
                    }
                }
            }

            if (sum_w < 1e-9f) continue;

            float new_x = sum_wx / sum_w;
            float new_y = sum_wy / sum_w;

            float dx    = new_x - g->Vertices[i]->x;
            float dy    = new_y - g->Vertices[i]->y;
            float delta = sqrtf(dx*dx + dy*dy);
            if (delta > max_delta) max_delta = delta;

            g->Vertices[i]->x = new_x;
            g->Vertices[i]->y = new_y;
        }

        if (max_delta < TUTTE_EPSILON) break;
    }
}

/* ---------------------------------------------------------------
 * Właściwy algorytm Tutte'a z poprawkami
 *
 * boundary_count_hint - ignorowany (liczymy cykl sami)
 * max_iter            - maks iteracje Gaussa-Seidela (0 = domyślne)
 * --------------------------------------------------------------- */
void ForceFieldSim(Graph g, int boundary_count_hint, int max_iter) {
    (void)boundary_count_hint;

    if (g->v_count == 0) return;
    if (max_iter <= 0) max_iter = TUTTE_MAX_ITER;

    /* --- 1. Wykryj składowe spójne, operuj na największej --- */
    char* in_component = (char*)calloc(g->v_count, 1);
    if (!in_component) return;

    /* Znajdź największą składową spójną przez BFS */
    int best_start = 0, best_size = 0;
    char* visited_tmp = (char*)calloc(g->v_count, 1);

    if (visited_tmp) {
        for (int s = 0; s < g->v_count; s++) {
            if (visited_tmp[s]) continue;
            char* vis = (char*)calloc(g->v_count, 1);
            if (!vis) break;
            bfs_reachable(g, s, vis);
            int sz = 0;
            for (int i = 0; i < g->v_count; i++) sz += vis[i];
            if (sz > best_size) { best_size = sz; best_start = s; }
            for (int i = 0; i < g->v_count; i++) visited_tmp[i] |= vis[i];
            free(vis);
        }
        free(visited_tmp);
    }

    /* Oznacz wierzchołki spoza największej składowej — będą pominięte */
    bfs_reachable(g, best_start, in_component);

    /* Wierzchołki izolowane/spoza składowej → umieść z boku */
    for (int i = 0; i < g->v_count; i++) {
        if (!in_component[i]) {
            g->Vertices[i]->x = 0.0f;
            g->Vertices[i]->y = 0.0f;
        }
    }

    /* --- 2. Znajdź zewnętrzny cykl w głównej składowej --- */
    int* outer_cycle = (int*)malloc(g->v_count * sizeof(int));
    if (!outer_cycle) { free(in_component); return; }

    int k = find_outer_cycle(g, outer_cycle);

    /* --- 3. Rozmieść wierzchołki graniczne równomiernie na okręgu --- */
    char* fixed = (char*)calloc(g->v_count, sizeof(char));
    if (!fixed) { free(outer_cycle); free(in_component); return; }

    for (int i = 0; i < k; i++) {
        int idx   = outer_cycle[i];
        float angle = 2.0f * (float)M_PI * i / (float)k;
        g->Vertices[idx]->x = TUTTE_RADIUS * cosf(angle);
        g->Vertices[idx]->y = TUTTE_RADIUS * sinf(angle);
        fixed[idx] = 1;
    }

    /* --- 4. Inicjalizacja wewnętrznych: centroid sąsiadów --- */
    int* nb_buf = (int*)malloc((g->e_count * 2 + 1) * sizeof(int));
    if (!nb_buf) { free(fixed); free(outer_cycle); free(in_component); return; }

    for (int i = 0; i < g->v_count; i++) {
        if (fixed[i] || !in_component[i]) continue;

        int n = neighbours(g, i, nb_buf);
        if (n == 0) { g->Vertices[i]->x = g->Vertices[i]->y = 0.0f; continue; }

        float sx = 0.0f, sy = 0.0f;
        for (int j = 0; j < n; j++) {
            sx += g->Vertices[nb_buf[j]]->x;
            sy += g->Vertices[nb_buf[j]]->y;
        }
        g->Vertices[i]->x = sx / (float)n;
        g->Vertices[i]->y = sy / (float)n;
    }

    /* --- 5. Iteracje Gaussa-Seidela z wagami --- */
    gauss_seidel(g, fixed, nb_buf, max_iter);

    free(nb_buf);
    free(fixed);
    free(outer_cycle);
    free(in_component);
}
