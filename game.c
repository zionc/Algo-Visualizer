#include <stdio.h>
#include <stdlib.h>
#include "./include/raylib.h"
#include "./include/raymath.h"
#include "./include/graph.h"
#include <string.h>


#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500

typedef enum GameState {
    DRAWING,            // State where User is drawing
    ANIMATING,          // State where nodes/edges are animating
    PAUSED,
}GameState;

typedef enum NodeState {
    UNCONNECTED,     // Node has no neighbors
    CONNTECTED,      // Node has at least one neighbor
    DRAG,            // Node is currently being dragged
    UNVISITED,       // Node not visited during ANIMATING GameState
    VISITED,         // Node visited during ANIMATING GameState
    START,           // Start of path search
    END              // End of path search
}NodeState;

typedef struct Game {
    GameState state;            // State of the game
    Graph *g;               // Graph to create logic/connections
    Node *edge_start;       // Start of an edge
    Node *animate_start;    // Start of a path
    Node *animate_end;      // End of a path
    int edge_animate_index; // Current edge to animate
    float animate_speed;    // Speed of which to animate the edges
}Game;

void game_init(Game *game, Graph *graph, int speed)
{
    game->state = DRAWING;
    game->g     = graph;
    game->edge_start = NULL;
    game->animate_start = NULL;
    game->animate_end   = NULL;
    game->edge_animate_index = 0;
    game->animate_speed      = speed; 
}

// Check and returns Node if mouse_pos collides with it, NULL if it doesn't
Node *game_node_collision_mouse(Game *game,Vector2 mouse_pos) 
{   
    Graph *g = game->g;
    for(int i = 0; i < g->nodes_pool_size; i++) {
        Node *node = g->nodes_pool[i];
        if(CheckCollisionPointCircle(mouse_pos,node->position,CIRCLE_RADIUS))
            return node;
    }
    return NULL;
}

// Create node on mouseclick
void game_create_node(Game *game) 
{
    Graph *g = game->g;
    Vector2 position = GetMousePosition();
    graph_create_node(g,position,g->edges_pool_size);
}

void game_create_edge(Game *game, Node *start, Node *end)
{
    int distance = Vector2Distance(start->position,end->position);
    graph_connect_weight(game->g,start,end,distance);
    graph_connect_weight(game->g,end,start,distance);
}








int main()
{
    printf("Hello, World!\n");
    return 0;
}