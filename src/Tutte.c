#include "Tutte.h"
#include "graph.h"
#include "vertex.h"
#include "Vector2D.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TUTTE_RADIUS   100.0f
#define TUTTE_EPSILON  1e-6f

/* Zwraca indeks wierzchołka o danym id, lub -1 */
static int find_vertex_index(Graph g, int id) {
    	for (int i = 0; i < g->v_count; i++)
        	if (g->Vertices[i]->id == id) return i;
    	
	return -1;
}

/* Wypełnia tablicę neighbour_ids sąsiadami wierzchołka o indeksie v_idx.
   Zwraca liczbę sąsiadów. */
static int get_neighbours_by_idx(Graph g, int v_idx,
                                  int* neighbour_idx, int max_n) {
    	
	int id  = g->Vertices[v_idx]->id;
    	int cnt = 0;
    	
    	for (int i = 0; i < g->e_count && cnt < max_n; i++) {
        	Edge e = g->Edges[i];
        	if (e->A->id == id) {
            		int nb = find_vertex_index(g, e->B->id);
            		if (nb != -1) neighbour_idx[cnt++] = nb;
        	} else if (e->B->id == id) {
           	 	int nb = find_vertex_index(g, e->A->id);
            		if (nb != -1) neighbour_idx[cnt++] = nb;
        	}
    	}
    	return cnt;
}

/* ---------------------------------------------------------------
 * Znajdź cykl w grafie metodą DFS i zwróć go jako tablicę indeksów.
 * Szukamy najdłuższego prostego cyklu startując od każdego węzła,
 * żeby jako zewnętrzna ściana wybrać możliwie duży cykl.
 * Zwraca długość cyklu (>= 3) lub 0 gdy nie znaleziono.
 * cycle_out musi mieć rozmiar >= v_count.
 * --------------------------------------------------------------- */
static int find_cycle_dfs(Graph g, int start_idx,
                           int* path, int path_len,
                           char* visited,
                           int* best, int* best_len) {
    	int nb[1024];
    	int n = get_neighbours_by_idx(g, start_idx, nb, 1024);

    	for (int i = 0; i < n; i++) {
        	int nbr = nb[i];

        	/* Zamknięcie cyklu: sąsiad jest już na ścieżce (i nie jest bezpośrednim rodzicem) */
        	if (visited[nbr]) {
            		/* Znajdź gdzie nbr jest na ścieżce */
            		for (int j = 0; j < path_len; j++) {
                		if (path[j] == nbr) {
                    			int cycle_len = path_len - j;
		    
                    			if (cycle_len >= 3 && cycle_len > *best_len) {
                        			*best_len = cycle_len;
                        			memcpy(best, path + j, cycle_len * sizeof(int));
                    			}

                    			break;

                		}
            		}
            		continue;
        	}

        	visited[nbr] = 1;
        	path[path_len] = nbr;
        	find_cycle_dfs(g, nbr, path, path_len + 1, visited, best, best_len);
        	visited[nbr] = 0;
    	}
    	return *best_len;
}

static int find_outer_cycle(Graph g, int* cycle_out) {
    	if (g->v_count == 0) return 0;

    	char* visited = (char*)calloc(g->v_count, 1);
    	int*  path    = (int*)malloc(g->v_count * sizeof(int));
    	int*  best    = (int*)malloc(g->v_count * sizeof(int));
    	int   best_len = 0;

    	if (!visited || !path || !best) {
        	free(visited); free(path); free(best);
        	return 0;
    	}

    	/* Próbuj od wierzchołka o największym stopniu — zwiększa szansę
       	na duży cykl = lepszą zewnętrzną ścianę */
    	
    	int start = 0, max_deg = 0;
    	int* tmp_nb = (int*)malloc(g->e_count * 2 * sizeof(int));
    	
    	for (int i = 0; i < g->v_count; i++) {
        	int d = get_neighbours_by_idx(g, i, tmp_nb, g->e_count * 2);
        	if (d > max_deg) { max_deg = d; start = i; }
    	}

    	free(tmp_nb);

    	visited[start] = 1;
    	path[0] = start;
    	find_cycle_dfs(g, start, path, 1, visited, best, &best_len);

    	if (best_len >= 3) memcpy(cycle_out, best, best_len * sizeof(int));

    	free(visited); free(path); free(best);
    	return best_len;
}

/* ---------------------------------------------------------------
 * Właściwy algorytm Tutte'a
 * --------------------------------------------------------------- */
void ForceFieldSim(Graph g, int boundary_count_hint, int max_iter) {
    	(void)boundary_count_hint;   /* ignorujemy hint — liczymy cykl sami */

    	if (g->v_count == 0) return;

    	/* --- 1. Znajdź zewnętrzną ścianę (cykl) --- */
    	int* outer_cycle = (int*)malloc(g->v_count * sizeof(int));
    	if (!outer_cycle) return;

    	int k = find_outer_cycle(g, outer_cycle);

    	if (k < 3) {
        	/* Graf nie ma cyklu (drzewo?) — po prostu rozmieść wszystko na okręgu */
        	k = g->v_count;
        	for (int i = 0; i < k; i++) outer_cycle[i] = i;
    	}

    	/* --- 2. Oznacz wierzchołki graniczne --- */
    	char* fixed = (char*)calloc(g->v_count, sizeof(char));
    	if (!fixed) { free(outer_cycle); return; }

    	for (int i = 0; i < k; i++) {
        	float angle = 2.0f * (float)M_PI * i / (float)k;
        	g->Vertices[outer_cycle[i]]->x = TUTTE_RADIUS * cosf(angle);
        	g->Vertices[outer_cycle[i]]->y = TUTTE_RADIUS * sinf(angle);
        	fixed[outer_cycle[i]] = 1;
    	}

    	/* --- 3. Pozostałe wierzchołki: startują w centroidzie sąsiadów --- */
    	int* nb_buf = (int*)malloc((g->e_count * 2 + 1) * sizeof(int));
    	if (!nb_buf) { free(fixed); free(outer_cycle); return; }

    	/* Inicjalizacja: każdy swobodny wierzchołek = średnia pozycja sąsiadów
       		(tych, którzy mają już ustawione pozycje, czyli granicznych) */
    	for (int i = 0; i < g->v_count; i++) {
        	if (fixed[i]) continue;
        	int n = get_neighbours_by_idx(g, i, nb_buf, g->e_count * 2);
        	
		if (n == 0) {
            		g->Vertices[i]->x = 0.0f;
            		g->Vertices[i]->y = 0.0f;
            		continue;
        	}

        	float sx = 0, sy = 0;
        	for (int j = 0; j < n; j++) {
            		sx += g->Vertices[nb_buf[j]]->x;
            		sy += g->Vertices[nb_buf[j]]->y;
        	}
        	g->Vertices[i]->x = sx / n;
        	g->Vertices[i]->y = sy / n;
    	}

    /* --- 4. Iteracje Tutte (Gaussa-Seidela) --- */
    	for (int iter = 0; iter < max_iter; iter++) {
        	float max_delta = 0.0f;

        	for (int i = 0; i < g->v_count; i++) {
            		if (fixed[i]) continue;

            		int n = get_neighbours_by_idx(g, i, nb_buf, g->e_count * 2);
            		if (n == 0) continue;

            		float sum_x = 0.0f, sum_y = 0.0f;
            		for (int j = 0; j < n; j++) {
                		sum_x += g->Vertices[nb_buf[j]]->x;
                		sum_y += g->Vertices[nb_buf[j]]->y;
            		}

            		float new_x = sum_x / (float)n;
            		float new_y = sum_y / (float)n;

            		float dx = new_x - g->Vertices[i]->x;
            		float dy = new_y - g->Vertices[i]->y;
           	 	float delta = sqrtf(dx*dx + dy*dy);
            		if (delta > max_delta) max_delta = delta;

            			g->Vertices[i]->x = new_x;
            			g->Vertices[i]->y = new_y;
        	}

        	if (max_delta < TUTTE_EPSILON) break;
    		
    	}

    	free(nb_buf);
    	free(fixed);
    	free(outer_cycle);
}
