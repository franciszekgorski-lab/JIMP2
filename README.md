# Graph Layout Engine

A C program that reads a weighted graph from a file and computes a 2D layout using one of two algorithms: a force-directed simulation with inextensible edge constraints, or the Fruchterman-Reingold algorithm. The result can be saved in text, binary, or visualizer-ready format.

---

## Building

```bash
gcc -o graph_layout main.c graph.c vertex.c edge.c Vector2D.c simulation.c fruchterman-reingold.c Tutte.c -lm
```

---

## Usage

```
./graph_layout -i <input> -o <output> [options]
```

| Flag | Argument | Description |
|------|----------|-------------|
| `-i` | `<file>` | Input graph file (required) |
| `-o` | `<file>` | Output file (required) |
| `-a` | `fruchterman` or `tutte` | Layout algorithm (default: `tutte`) |
| `-f` | `txt` or `bin` | Output format (default: `bin`) |
| `-v` | — | Write output in visualizer format |
| `-p` | — | Print graph to stdout after layout |

### Examples

```bash
# Fruchterman-Reingold, text output
./graph_layout -i mygraph.txt -o out.txt -a fruchterman -f txt

# Force simulation, visualizer output
./graph_layout -i mygraph.txt -o out.vis -v

# Print result to console
./graph_layout -i mygraph.txt -o out.txt -f txt -p
```

---

## Input Format

Plain text file, one edge per line:

```
<edge_id> <vertex_A_id> <vertex_B_id> <weight>
```

Example (`wheel`):
```
1 1 2 1.0
2 2 3 1.0
3 3 4 1.0
4 4 5 1.0
5 5 6 1.0
6 6 1 1.0
7 7 1 1.0
8 7 2 1.0
9 7 3 1.0
10 7 4 1.0
11 7 5 1.0
12 7 6 1.0
```

Vertices that appear in edges but haven't been declared yet are created automatically.

---

## Output Formats

**Text** (`-f txt`): one vertex per line — `id x y`

**Binary** (`-f bin`, default): raw `fwrite` of the vertex pointer array

**Visualizer** (`-v`): structured text format readable by an external visualizer tool:
```
<vertex_count> <edge_count>
<x> <y>          ← one line per vertex
...
<idx_A> <idx_B>  ← one line per edge (0-based indices)
...
```

---

## Algorithms

### Force Simulation (`tutte`, default)

A physics-based simulation where vertices repel each other like electric charges (Coulomb's law), and edges act as **inextensible threads** — they allow vertices to move closer but prevent them from exceeding a maximum length. The simulation runs until the total kinetic energy of the system drops below a threshold, indicating the graph has reached equilibrium.

Key parameters (defined in `simulation.c`):

| Constant | Value | Meaning |
|----------|-------|---------|
| `k` | `0.1` | Repulsion strength |
| `damping` | `0.7` | Velocity damping per step |
| `MIN_VEL` | `0.001` | Energy threshold for stop condition |
| `RADIUS` | `5` | Starting circle radius |
| `MAX_EDGE_LEN` | `8.0` | Maximum allowed edge length |

### Fruchterman-Reingold (`-a fruchterman`)

A classical force-directed layout algorithm. Vertices repel each other globally (force ∝ `k²/dist`) while edges attract connected pairs (force ∝ `dist²/k`, scaled by edge weight). A temperature parameter limits the maximum displacement per step and decreases over time, acting as a cooling schedule. Final positions are normalized to `[0, 1]`.

### Tutte Embedding (internal, `ForceFieldSim`)

A proper Tutte embedding: the largest biconnected component is detected via BFS, its outer cycle is placed on a circle, and interior vertices are iteratively placed at the weighted average of their neighbors using Gauss-Seidel relaxation.

---

## Project Structure

```
.
├── main.c                  # Argument parsing, top-level control flow
├── graph.c / graph.h       # Graph structure, loading, saving
├── vertex.c / vertex.h     # Vertex struct and movement
├── edge.c / edge.h         # Edge struct
├── Vector2D.c / Vector2D.h # 2D vector type
├── simulation.c            # Force simulation with edge constraints
├── fruchterman-reingold.c  # Fruchterman-Reingold algorithm
└── Tutte.c / Tutte.h       # Tutte embedding
```

---

## Return Value

`main` returns a bitmask encoding which steps were skipped or failed:

| Bit | Meaning |
|-----|---------|
| 0 | No input file specified |
| 1 | Algorithm used was `tutte`/simulation (not Fruchterman) |
| 2 | No output file specified |
| 3 | Invalid `-f` format argument |
| 4 | Invalid `-a` algorithm argument |
| 5 | Unknown flag passed |
| 6 | Input file not found |
