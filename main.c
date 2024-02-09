#include <stdio.h>
#include <stdlib.h>
#include "include/raylib.h"
#include "include/raymath.h"
#include "include/graph.h"

#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500


// Graph entry point
Graph g = {0};


// Check and returns Node if mouse_pos collides with it, NULL if it doesn't
Node *check_node_collision_mouse(Vector2 mouse_pos) 
{
    for(int i = 0; i < g.nodes_pool_size; i++) {
        Node *node = g.nodes_pool[i];
        if(CheckCollisionPointCircle(mouse_pos,node->position,CIRCLE_RADIUS))
            return node;
    }
    return NULL;
}


// Display edges and nodes
// TODO - A lot going on here...
void display_graph() 
{
    if(g.nodes_pool_size == 0) {
        printf("No nodes drawn\n");
        return;
    }
    printf("--------------------- Nodes ---------------------\n");
    for(int i = 0; i < g.nodes_pool_size; i++) {
        Node *node = g.nodes_pool[i];
        printf("Node [%d] Size = %d -> [",node->id,node->adjacent_size);
        for(int j = 0; j < node->adjacent_size; j++) {
            if(j == node->adjacent_size -1) printf("%d",node->neighbors[j].id);
            else printf("%d,",node->neighbors[j].id);
        }
        printf("]\n");   
    }
    printf("--------------------- Edges ---------------------\n");
    printf("Number of edges: %d\n",g.edges_pool_size);
    for(int i = 0; i < g.edges_pool_size; i++) {
        Edge *edge = g.edges_pool[i];
        printf("%d <---> %d (Weight: %d)\n",edge->node_from->id,edge->node_to->id,edge->weight);
    }
}


void register_new_node() 
{
    Vector2 vect = {
        .x = GetMouseX(),
        .y = GetMouseY()
    };
    graph_create_node(&g,vect,g.nodes_pool_size,UNCONNECTED);
}

void register_new_edge(Node *start, Node *end)
{
    int distance = Vector2Distance(start->position,end->position);
    graph_connect_weight(&g,start,end,distance);
    graph_connect_weight(&g,end,start,distance);
}

void draw_nodes()
{
    for(int i = 0; i < g.nodes_pool_size; i++) {
        Node *node = g.nodes_pool[i];
        NodeState state = node->state;
        switch (state)
        {
        case UNCONNECTED:
            node->color = RED;
            break;
        case CONNECTED:
            node->color = BLUE;
            break;
        case SELECTED:
            node->color = YELLOW;
            break;
        default:
            break;
        }
        DrawCircleV(node->position,CIRCLE_RADIUS,node->color);
    }
}

void draw_edges() 
{
    for(int i = 0; i < g.edges_pool_size; i++) {
        Edge edge = *g.edges_pool[i];
        DrawLineEx(edge.node_from->position,edge.node_to->position,CIRCLE_RADIUS/7,RAYWHITE);
    }
}

void draw_node_text()
{
    for(int i = 0; i < g.nodes_pool_size; i++) {
        Node node = *g.nodes_pool[i];
        char id[4] = {0};
        sprintf(id,"%d",node.id);

        Font default_font = GetFontDefault();
        Vector2 text_size = MeasureTextEx(default_font,id,CIRCLE_RADIUS,5);
        Vector2 scale = Vector2Scale(text_size,.5f);

        DrawTextEx(default_font,id,Vector2Subtract(node.position,scale),CIRCLE_RADIUS,5,GREEN);
    }
}

void update_edge_weights() 
{
    for(int i = 0; i < g.edges_pool_size; i++) {
        Edge *edge = g.edges_pool[i];
        int weight = Vector2Distance(edge->node_from->position,edge->node_to->position);
        edge->weight = weight;
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Algo Visualizer");
    SetTargetFPS(240);

    graph_init(&g,MAX_NODES);

    Node *edge_start = NULL;
    bool state_Moving_Nodes = false;
    Node *clicked_node = NULL;
    //
    // Game loop --------------------------------------------------------------
    while (!WindowShouldClose())
    {
        //
        // Update ---------------------------------------------------------------------
        
        // Edge Logic
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse_pos = GetMousePosition();
            clicked_node = check_node_collision_mouse(mouse_pos);

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                state_Moving_Nodes = true;
            }

            if(clicked_node != NULL && edge_start == NULL) {          
                clicked_node->state = SELECTED;
                edge_start  = clicked_node;
            } 
            else if(clicked_node != NULL && edge_start != NULL && edge_start != clicked_node) {     
                clicked_node->state = CONNECTED;
                edge_start->state   = CONNECTED;
                register_new_edge(edge_start,clicked_node);
                edge_start = NULL;
            }
            else if(clicked_node == NULL && edge_start != NULL) {
                if(edge_start->adjacent_size == 0) 
                    edge_start->state = UNCONNECTED;
                else
                    edge_start->state = CONNECTED;
                edge_start = NULL;
            }
            else if(clicked_node == NULL && edge_start == NULL) {
                register_new_node();
            }
        }

        // Drag logic
        if(state_Moving_Nodes && clicked_node) {
            clicked_node->position = GetMousePosition();
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                state_Moving_Nodes = false;
            }
        }
        
        // update weights
        update_edge_weights();

        // Show graph stdout
        if(IsKeyPressed(KEY_S)) {
            display_graph();
        }

        // ----------------------------------------------------------------------------
        // Draw
        BeginDrawing();
            ClearBackground(BACKGROUNDCOLOR);
            draw_edges();
            draw_nodes();
            draw_node_text();
            DrawFPS(15,15);
        EndDrawing();
    }
    graph_destroy(&g);
    CloseWindow();

    return 0;
}
