#include "graph.h"
#include <stdlib.h>
#include <stdio.h>


// #define MAX_NODES 500
// #define MAX_EDGES MAX_NODES * (MAX_NODES -1) / 2

// TODO: maybe use ifdef to create a global Graph??? Could be easier than placing graphs into
// every function.



void graph_init(Graph *graph,int max_nodes) 
{
    graph->max_nodes = max_nodes;
    graph->max_edges = max_nodes*(max_nodes-1);
    graph->nodes_pool = malloc(sizeof(Node) * graph->max_nodes);
    graph->edges_pool = malloc(sizeof(Edge) * graph->max_edges);
    graph->nodes_pool_size = 0;
    graph->edges_pool_size = 0;
}

// Free the heap
void graph_destroy(Graph *graph) 
{
    for(int i = 0; i < graph->nodes_pool_size; i++) {
        free(graph->nodes_pool[i]->neighbors);
    }
    free(graph->nodes_pool);
    free(graph->edges_pool);
}


static int graph_add_node(Graph *graph,Node *node) 
{
    
    graph->nodes_pool[graph->edges_pool_size] = node;
    graph->nodes_pool_size++;
    return 0;
}

static int graph_add_edge(Graph *graph,Edge* edge)
{
    graph->edges_pool[graph->edges_pool_size] = edge;
    graph->edges_pool_size++;
    return 0;
} 

static Edge graph_create_edge_weight(Graph *graph,Node* from, Node* to, int weight)
{
    Edge edge = {from,to,weight};
    graph_add_edge(graph,&edge);
    return edge;
}


Node graph_create_node_info(Graph *graph,void* info, int default_adjacency_size) {
    if(default_adjacency_size > graph->max_nodes) 
    {
        printf("Default adjacency size (%d) > MAX_NODES (%d)\n",default_adjacency_size,graph->max_nodes);
        exit(1);
    }
    Node* neighbors = malloc(sizeof(Node) * default_adjacency_size);
    if(neighbors == NULL)
    {
        printf("Could not create node\n");
        exit(1);
    }

    Node node = {.neighbors = neighbors, .info = info, .adjacent_size = 0};
    graph_add_node(graph,&node);
    return node;
}

Node graph_create_node(Graph *graph,int default_adjacency_size) 
{
    return graph_create_node_info(graph,NULL,default_adjacency_size);
}


void graph_connect_weight(Graph *graph,Node* from, Node* to, int weight)
{
    from->neighbors[from->adjacent_size] = *to;
    from->adjacent_size++;
    graph_create_edge_weight(graph,from,to,weight);
}

void graph_connect(Graph *graph,Node* from, Node* to) 
{
    graph_connect_weight(graph,from,to,0);
}

int graph_node_pool_contains(Graph *graph, Node* node) 
{
    for(int i = 0; i < graph->nodes_pool_size; i++) {
        if(graph_node_equals(graph->nodes_pool[i], node))
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

Node* graph_node_adjacents(Graph *graph,Node* node) 
{
    for(int i = 0; i < graph->nodes_pool_size; i++) {
        if(graph_node_equals(graph->nodes_pool[i],node)) {
            return graph->nodes_pool[i]->neighbors;
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


int main (void) {
    printf("Hello world!\n");
}