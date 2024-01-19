#ifndef GRAPH_H
#define GRAPH_H
#endif

struct Node;
struct Edge;
extern int nodes_size,edges_size;

typedef struct Node
{
    struct Node *neighbors;
    int adjacent_size;
    void* info;
} Node;

typedef struct Edge
{
    Node *node_from, *node_to;
    int weight;
} Edge;

extern Node* nodes_pool[];
extern Edge* edges_pool[];

void graph_connect(Node* from, Node* to);
void graph_connect_weight(Node* from, Node* to, int weight);
Node graph_create_node_info(void* info, int default_adjacency_size);
Node graph_create_node(int default_adjacency_size);

// Edge graph_create_edge_weight(Node* from, Node* to, int weight);
// Edge graph_create_edge(Node* from, Node* to);

int graph_edge_pool_size();
// int graph_add_node(Node* node);
// int graph_add_edge(Edge* edge);

int   graph_node_pool_contains(Node* node);
int   graph_node_contains(Node* node_container, Node* node);
int   graph_node_equals(Node* node_1, Node* node_2);
int   graph_node_pool_size();
Node* graph_node_adjacents(Node* node);