#ifndef GRAPH_H
#define GRAPH_H
#include "raylib.h"
#include "raymath.h"

typedef enum {
    UNCONNECTED,  
    HOVERING,
    SELECTED,
    CONNECTED,
}NodeState;

typedef struct Node
{
    struct Node *neighbors;
    int adjacent_size;
    Vector2 position;
    Color color;
    int id;
    char *args;
    NodeState state;
}Node;

typedef struct Edge
{
    Node *node_from, *node_to;
    int weight;
} Edge;

typedef struct Graph
{
    Node **nodes_pool;
    Edge **edges_pool;
    int nodes_pool_size, edges_pool_size;
    int max_nodes, max_edges;
} Graph;



// Create edge between from and to
void graph_connect(Graph *graph,Node* from, Node* to);

// Create edge between from and to with weight
void graph_connect_weight(Graph *graph,Node* from, Node* to, int weight);

// Initialize graph with maximum number of nodes, max_nodes
void graph_init(Graph *graph,int max_nodes);

// Free the heap from the graph
void graph_destroy(Graph *graph);

// Create node 
Node *graph_create_node(Graph *graph,Vector2 position,int id, NodeState state);

// Create node with struct pointer, struct_p
Node *graph_create_node_args(Graph *graph, void *struct_p,int size_of_struct);

// Get the adjacent list of Nodes
Node* graph_node_adjacents(Graph *graph,Node* node);

// Check if node exists in graph
int graph_node_pool_contains(Graph *graph,Node* node);

// Check if node is adjacent to node_container
int graph_node_contains(Node* node_container, Node* node);

// Test node_1 and node_2 for equality
int graph_node_equals(Node* node_1, Node* node_2);


#endif