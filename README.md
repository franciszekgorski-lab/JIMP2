# Visualization of a planary graph

Simple program that calculates coordinates of vertices in a weighted graph. It uses two different algorithms, Myown and Fruchtermana-Reingold's algorithm.

## Myown - explanation
It's an 'algorythm' that I just thought would work. Physics based, it uses edges as streching strings/springs and by adding forces that I think would help make that graph planar. Briefly, every vertex is a source of force for every other vertex so that they repel eachother. I had problems that sometimes the graph would become stationary so I added a source of force in the center of the graph too. It works for now, but needs more testing.
