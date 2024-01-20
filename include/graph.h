#ifndef GRAPH_H
#define GRAPH_H

typedef struct Node
{
    struct Node *neighbors;
    int adjacent_size;
    void* info;
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

// Create node with additional members specified by info
Node graph_create_node_info(Graph *graph,void* info, int default_adjacency_size);
Node graph_create_node(Graph *graph,int default_adjacency_size);
Node* graph_node_adjacents(Graph *graph,Node* node);

int   graph_node_pool_contains(Graph *graph,Node* node);
int   graph_node_contains(Node* node_container, Node* node);
int   graph_node_equals(Node* node_1, Node* node_2);


#endif