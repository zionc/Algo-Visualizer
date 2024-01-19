#include "graph.h"
#include <stdlib.h>


#define MAX_NODES 500
#define MAX_EDGES MAX_NODES * (MAX_NODES -1) / 2

Node* nodes_pool[MAX_NODES] = {0};
Edge* edges_pool[MAX_EDGES] = {0};
int nodes_size,edges_size = 0;

static int graph_add_node(Node* node) 
{
    nodes_pool[nodes_size] = node;
    nodes_size++;
    return 0;
}

static int graph_add_edge(Edge* edge)
{
    edges_pool[edges_size] = edge;
    edges_size++;
    return 0;
} 

static Edge graph_create_edge_weight(Node* from, Node* to, int weight)
{
    Edge edge = {from,to,weight};
    graph_add_edge(&edge);
    return edge;
}


Node graph_create_node_info(void* info, int default_adjacency_size) {
    if(default_adjacency_size > MAX_NODES) 
    {
        printf("Default adjacency size (%d) > MAX_NODES (%d)\n",default_adjacency_size,MAX_NODES);
        exit(1);
    }
    Node* neighbors = malloc(sizeof(Node) * default_adjacency_size);
    if(neighbors == NULL)
    {
        printf("Could not create node\n");
        exit(1);
    }

    Node node = {.neighbors = neighbors, .info = info, .adjacent_size = 0};
    graph_add_node(&node);
    return node;
}

Node graph_create_node(int default_adjacency_size) 
{
    return graph_create_node_info(NULL,default_adjacency_size);
}


void graph_connect_weight(Node* from, Node* to, int weight)
{
    from->neighbors[from->adjacent_size] = *to;
    from->adjacent_size++;
    graph_create_edge_weight(from,to,weight);
}

void graph_connect(Node* from, Node* to) 
{
    graph_connect_weight(from,to,0);
}

int graph_node_pool_contains(Node* node) 
{
    for(int i = 0; i < nodes_size; i++) {
        if(graph_node_equals(nodes_pool[i], node))
            return 1;
    }
    return 0;
}

int graph_node_contains(Node* node_container, Node* node) 
{
    for(int i = 0; i < node_container->adjacent_size; i++) {
        if(graph_node_equals(&node_container->neighbors[i],node)) return 1;
    }
    return 0;
}

Node* graph_node_adjacents(Node* node) 
{
    for(int i = 0; i < nodes_size; i++) {
        if(graph_node_equals(nodes_pool[i],node)) {
            return nodes_pool[i]->neighbors;
        }
    }
    return 0;
}

int graph_node_equals(Node* node_1, Node* node_2) {
    if (node_1->neighbors == node_2->neighbors && 
            node_1->adjacent_size == node_2->adjacent_size &&
            node_1->info == node_2->info) {return 1;}
    return 0;
}

void graph_clean() {
    for(int i = 0; i < nodes_size; i++) {
        free(nodes_pool[i]->neighbors);
    }
}

int graph_node_pool_size(){return nodes_size;}
int graph_edge_pool_size(){return edges_size;}

