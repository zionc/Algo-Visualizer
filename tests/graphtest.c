#include "../include/graph.h"
#include <stdio.h>
#include <assert.h>


typedef struct args{
    int id;
    char *label;
} args;

void test_create_node()
{
    Graph g = {0};
    graph_init(&g,100,NULL);
    Node node1 = graph_create_node(&g,10);
    Node node2 = graph_create_node(&g,30);
    Node node3 = graph_create_node(&g,10);
    assert(g.nodes_pool_size == 3);
    assert(g.edges_pool_size == 0);
    assert(graph_node_pool_contains(&g,&node1));
    assert(graph_node_pool_contains(&g,&node2));
    assert(graph_node_pool_contains(&g,&node3));
    graph_destroy(&g);
}

void test_graph_connect()
{
    Graph g = {0};
    graph_init(&g,100,NULL);
    Node node1 = graph_create_node(&g,2);
    Node node2 = graph_create_node(&g,5);
    Node node3 = graph_create_node(&g,10);
    assert(node1.adjacent_size == 0);
    assert(node2.adjacent_size == 0);
    assert(node3.adjacent_size == 0);

    graph_connect(&g,&node1,&node2);
    graph_connect(&g,&node1,&node3);
    assert(node1.adjacent_size == 2);
    assert(graph_node_contains(&node1,&node2));
    assert(graph_node_contains(&node1,&node3));
    assert(!graph_node_contains(&node2,&node1));
    assert(!graph_node_contains(&node3,&node1));
    assert(g.edges_pool_size == 2);

    graph_connect(&g,&node2,&node3);
    assert(node2.adjacent_size == 1);
    assert(node1.adjacent_size == 2);
    assert(node3.adjacent_size == 0);
    assert(graph_node_contains(&node2,&node3));
    assert(!graph_node_contains(&node3,&node2));

    graph_connect(&g,&node2,&node1);
    assert(graph_node_contains(&node2,&node1));

    assert(g.nodes_pool_size == 3);
    assert(g.edges_pool_size == 4);
    

    graph_destroy(&g);
    
}

void test_node_equal()
{
    Graph g = {0};
    graph_init(&g,100,NULL);

    Node node1 = graph_create_node(&g,5);
    Node node2 = graph_create_node(&g,5);
    Node node3 = graph_create_node(&g,100);
    assert(!graph_node_equals(&node1,&node2));
    assert(graph_node_equals(&node1,&node1));
    assert(!graph_node_equals(&node3,&node2));

    graph_destroy(&g);
}  

// void test_graph_add_info() {
//     Graph g = {0};
//     graph_init(&g,500);

//     graph_destroy(&g);
// }

int main(void) {
    test_create_node();
    test_graph_connect();
    test_node_equal();
}