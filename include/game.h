#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include "graph.h"

typedef enum GameState {
    DRAWING,      // State where User is drawing (creating nodes/edges, dragging)
    ANIMATING,    // State where nodes/edges are animating
    PAUSED,       // Game is paused (during ANIMATING state)
    DRAGGING,     // User is dragging nodes
}GameState;


typedef struct Game {
    GameState state;        // State of the game
    Graph *g;               // Game's graph
    Node *edge_start;       // Node to draw an edge from
    Node *animate_start;    // Start of a path visualizer 
    Node *animate_end;      // End of a path visualizer
    int edge_animate_index; // Integer points to current edge that is being animated
    float animate_speed;    // Amount to increment an edge by per frame
    float animate_distance; // Total distance an edge has been drawn, must be 0 <= animate_distance <= edge's weight
}Game;


void game_init(Game *game, Graph *graph, int speed); // Initialize game with graph and speed of edge animation


// Update -----------------------------------------------
void game_update_all_node_state_drawing(Game *game);            // Update the states of all nodes based on Drawing/Dragging state
void game_update_node_state(Node *node);                        // Update the state of one node based on Drawing/Dragging state
void game_update_edge_weights(Game *game);                      // Update weights for nodes (Needed if nodes move around)
void game_update_left_click(Game *game);                        // Update Game on left click
void game_update_left_drag(Game *game);                         // Update Game on drag
void game_create_edge(Game *game, Node *start, Node *end);      // Create an edge between start and end
void game_create_node(Game *game);                              // Create node on left click         
Node *game_update_node_collision(Game *game,Vector2 mouse_pos); // Check and returns Node if mouse_pos collides with it, NULL if it doesn't


// Draw   -----------------------------------------------
void game_draw_nodes(Game *game);   // Draw Nodes, color depends on state
void game_draw_node_id(Game *game); // Draw id's for nodes
void game_draw_edges(Game *game);   // Draw edges

// Animate ----------------------------------------------
void game_animate_search_edges(Game *game, Edge **edges); // Animate edges traversed from search algorithms


#endif