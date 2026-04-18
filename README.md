# Graph Layout Engine

A C program that reads a weighted graph from a file and computes a 2D layout using one of two algorithms: a force-directed simulation with inextensible edge constraints, or the Fruchterman-Reingold algorithm. The result can be saved in text, binary, or visualizer-ready format.

---

## Building

Requires `gcc` and `make`. The `obj/` directory is created automatically.

```bash
make
```

To clean build artifacts:

```bash
make clean
```

The Makefile compiles all `src/*.c` files, looks for headers in `headers/`, links with `-lm`, and produces the `main` executable.

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
./main -i graphs/wheel -o out.txt -a fruchterman -f txt

# Force simulation, visualizer output
./main -i graphs/wheel -o out.vis -v

# Print result to console
./main -i graphs/wheel -o out.txt -f txt -p
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

### Force Simulation — inspired by Tutte (`tutte`, default)

A physics-based simulation inspired by the Tutte embedding idea of placing vertices relative to their neighbors. Vertices repel each other like electric charges (Coulomb's law), and edges act as **inextensible threads** — they allow vertices to move closer together but prevent them from exceeding a maximum length. When a constraint is violated, the positions are corrected and the outward velocity components are zeroed out. The simulation runs until the total kinetic energy of the system drops below a threshold, indicating equilibrium has been reached.

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

---

## Project Structure

```
.
├── Makefile
├── main                        # Skompilowany plik wykonywalny (po make)
├── data                        # Przykładowe dane wyjściowe
├── visual                      # Narzędzie do wizualizacji
├── src/                        # Pliki źródłowe
│   ├── main.c                  # Parsowanie argumentów, główny przepływ
│   ├── graph.c                 # Struktura grafu, wczytywanie, zapisywanie
│   ├── vertex.c                # Wierzchołek: konstruktor, ruch
│   ├── edge.c                  # Krawędź: konstruktor
│   ├── Vector2D.c              # Typ wektora 2D
│   ├── simulation.c            # Symulacja fizyczna inspirowana Tuttem
│   └── fruchterman-reingold.c  # Algorytm Fruchtermana-Reingolda
├── headers/                    # Pliki nagłówkowe
│   ├── graph.h
│   ├── vertex.h
│   ├── edge.h
│   └── Vector2D.h
├── obj/                        # Pliki .o (tworzone automatycznie przez make)
└── graphs/                     # Przykładowe grafy wejściowe
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
