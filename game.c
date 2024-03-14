#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
/**
 * game.c handles rendering nodes/animations to the screen
 * 
 * @author Zion Chilagan
 */

#define CIRCLE_RADIUS 20.f
#define FONTSIZE 6


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
    graph_create_node(g,position,g->nodes_pool_size);
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
        if(node->adjacent_size > 0) node->state = DRAWING_CONNECTED;
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
        case DRAWING_CONNECTED:
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
            printf("[ERROR] Unknown STATE: COLOR match\n");
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

void game_update_left_click(Game *game)
{
    Vector2 mouse_pos  = GetMousePosition();
    Node *clicked_node = game_update_node_collision(game,mouse_pos);
    Node *edge_start   = game->edge_start;

    // Initiate user wants to draw an edge by selecting start node
    if(clicked_node != NULL && edge_start == NULL) {          
        clicked_node->state = DRAWING_SELECTED;
        game->edge_start  = clicked_node;
    } 

    // Create an edge, as long as the two nodes are different
    else if(clicked_node != NULL && edge_start != NULL && edge_start != clicked_node) {
        clicked_node->state = DRAWING_CONNECTED;
        edge_start->state   = DRAWING_CONNECTED;
        game_create_edge(game,edge_start,clicked_node);
        game->edge_start = NULL;
    }

    // If user clicks on empty space after initiaiting edge, cancel the process
    else if(clicked_node == NULL && edge_start != NULL) {
        if(edge_start->adjacent_size == 0) 
            edge_start->state = DRAWING_UNCONNECTED;
        else
            edge_start->state = DRAWING_CONNECTED;
        game->edge_start = NULL;
    }

    // Create a node
    else if(clicked_node == NULL && edge_start == NULL) {
        game_create_node(game);
    }
}

