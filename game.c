#include <stdio.h>
#include <stdlib.h>
#include "./include/raylib.h"
#include "./include/raymath.h"
#include "./include/graph.h"
#include <string.h>

/**
 * game.c handles rendering nodes/animations to the screen
 * 
 * @author Zion Chilagan
 */

#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500
#define FONTSIZE 5

typedef enum GameState {
    DRAWING,      // State where User is drawing (creating nodes/edges, dragging)
    ANIMATING,    // State where nodes/edges are animating
    PAUSED,       // Game is paused (during ANIMATING state)
}GameState;

typedef enum NodeState {
    DRAWING_UNCONNECTED,     // Node has no neighbors
    DRAWING_CONNTECTED,      // Node has at least one neighbor
    DRAWING_SELECTED,        // Node selected, initiated for drawing edge
    DRAWING_DRAG,            // Node is currently being dragged
    ANIMATING_UNVISITED,     // Node not visited during ANIMATING GameState
    ANIMATING_VISITED,       // Node visited during ANIMATING GameState
    ANIMATING_START,         // Start of path search
    ANIMATING_END            // End of path search
}NodeState;

typedef struct Game {
    GameState state;        // State of the game
    Graph *g;               // Graph to create logic/connections
    Node *edge_start;       // Node to draw an edge from
    Node *animate_start;    // Start of a path
    Node *animate_end;      // End of a path
    int edge_animate_index; // Current edge to animate
    float animate_speed;    // Speed of which to animate the edges
    float animate_distance; // Scale of how far out an animate edge is going
}Game;

// Initialize game with graph and speed of edge animation
void game_init(Game *game, Graph *graph, int speed)
{
    game->state = DRAWING;
    game->g     = graph;
    game->edge_start    = NULL;
    game->animate_start = NULL;
    game->animate_end   = NULL;
    game->edge_animate_index = 0;
    game->animate_speed      = speed;
    game->animate_distance   = 0;
}

// Check and returns Node if mouse_pos collides with it, NULL if it doesn't
Node *game_update_node_collision(Game *game,Vector2 mouse_pos) 
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

// Create an edge between start and end
void game_create_edge(Game *game, Node *start, Node *end)
{
    int distance = Vector2Distance(start->position,end->position);
    graph_connect_weight(game->g,start,end,distance);
    graph_connect_weight(game->g,end,start,distance);
}

// Set the Node state appropriately while in DRAWING state
void game_update_node_state_drawing(Game *game)
{
    Graph *g = game->g;
    for(int i = 0; i < g->nodes_pool_size; i++) {
        Node *node = g->nodes_pool[i];
        if(node->adjacent_size > 0) node->state = DRAWING_CONNTECTED;
        else node->state  = DRAWING_UNCONNECTED;
    }
}

// Draw Nodes, color depends on state
void game_draw_nodes(Game *game)
{
    Graph *g = game->g;
    for(int i = 0; i < g->nodes_pool_size; i++) {
        Node *node = g->nodes_pool[i];
        switch (node->state)
        {
        case DRAWING_CONNTECTED:
            node->color = BLUE;
            break;
        case DRAWING_UNCONNECTED:
            node->color = RED;
            break;
        case DRAWING_SELECTED:
            node->color = YELLOW;
            break;
        case ANIMATING_START:
            node->color = GREEN;
            break;
        case ANIMATING_END:
            node->color = GREEN;
            break;
        case ANIMATING_UNVISITED:
            node->color = GRAY;
            break;
        case ANIMATING_VISITED:
            node->color = RED;
            break;
        default:
            break;
        }
        DrawCircleV(node->position,CIRCLE_RADIUS,node->color);
    }
}

// Draw id's for nodes
void game_draw_node_id(Game *game)
{
    Graph *g = game->g;
    for(int i = 0; i < g->nodes_pool_size; i++) {
        Node *node = g->nodes_pool[i];
        char id[4] = {0};
        sprintf(id,"%d",node->id);

        Font default_font = GetFontDefault();
        Vector2 text_size = MeasureTextEx(default_font,id,CIRCLE_RADIUS,5);
        Vector2 scale = Vector2Scale(text_size,.5f);

        DrawTextEx(default_font,id,Vector2Subtract(node->position,scale),CIRCLE_RADIUS,FONTSIZE,BLACK);
    }
}

// Draw edges
void game_draw_edges(Game *game) 
{
    Graph *g = game->g;
    for(int i = 0; i < g->edges_pool_size; i++) {
        Edge edge = *g->edges_pool[i];
        DrawLineEx(edge.node_from->position,edge.node_to->position,CIRCLE_RADIUS/7,RAYWHITE);
    }
}

// Update weights for nodes (Needed if nodes move around)
void game_update_edge_weights(Game *game) 
{   
    Graph *g = game->g;
    for(int i = 0; i < g->edges_pool_size; i++) {
        Edge *edge = g->edges_pool[i];
        int weight = Vector2Distance(edge->node_from->position,edge->node_to->position);
        edge->weight = weight;
    }
}

// Animate edges traversed from search algorithms
void game_animate_search_edges(Game *game, Edge **edges) 
{
    // Draw paths we've already processed
    for(int i = 0; i < game->edge_animate_index; i++) {
        Edge *edge = edges[i];
        DrawLineEx(edge->node_from->position,edge->node_to->position,CIRCLE_RADIUS/3.5f,YELLOW);
    }

    // If there are no more edges to animate, return
    if(edges[game->edge_animate_index] == 0) {
        return;
    }

    // Animate current edge
    Edge *edge  = edges[game->edge_animate_index]; 
    float distance = game->animate_distance;
    Vector2 draw_to_vector = Vector2MoveTowards(edge->node_from->position, edge->node_to->position,distance);
    DrawLineEx(edge->node_from->position,draw_to_vector,CIRCLE_RADIUS/3.5f,YELLOW);

    // How fast we want to animate
    float scale = game->animate_speed;
    game->animate_distance+=scale;

    // Once an edge has been drawn out and overlaps with destination goal, move on to next edge
    if(game->animate_distance >= edge->weight) {
        game->edge_animate_index++;
        game->animate_distance = 0.f;
        if(edge->node_to->state != ANIMATING_END)
            edge->node_to->state = ANIMATING_VISITED;
    }
}



int main()
{
    printf("Hello, World!\n");
    return 0;
}