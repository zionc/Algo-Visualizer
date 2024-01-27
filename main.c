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

// Draw a singular Node with circle_color, and text as text_color
void draw_node(RenderTexture2D target, Node node, Color circle_color, Color text_color) 
{
    char id[4] = {0};
    sprintf(id,"%d",node.id);

    Font default_font = GetFontDefault();
    Vector2 text_size = MeasureTextEx(default_font,id,CIRCLE_RADIUS,5);
    Vector2 scale = Vector2Scale(text_size,.5f);

    BeginTextureMode(target);
    DrawCircleV(node.position, CIRCLE_RADIUS, circle_color);
    DrawTextEx(default_font,id,Vector2Subtract(node.position,scale),CIRCLE_RADIUS,5,text_color);
    EndTextureMode();
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


// Initiate drawing an edge, handles color
void select_edge_start(RenderTexture2D target,Node *start) 
{
    draw_node(target,*start,YELLOW,GREEN);
}

// Create an edge between start and end, also updating the neighbors
// TODO - Refactor this, maybe split up data processing and drawing
void select_edge_end(RenderTexture2D target, Node **start, Node **end, Vector2 mouse_pos) 
{   
    // Return if user clicks the start Node to be the end of the edge
    if(CheckCollisionPointCircle(mouse_pos,(*start)->position,CIRCLE_RADIUS)){
        if((*start)->adjacent_size)      
            draw_node(target,**start,GREEN,YELLOW);
        else                    
            draw_node(target,**start,RED,YELLOW);
        *start = NULL;
        return;
    }

    // Return if user doesn't click on a node
    *end = check_node_collision_mouse(mouse_pos);
    if(end == NULL) 
        return;

    // Calculate weight based on distance
    int distance = Vector2Distance((*start)->position,(*end)->position);
    graph_connect_weight(&g,*start,*end,distance);
    graph_connect_weight(&g,*end,*start,distance);


    BeginTextureMode(target);
    DrawLineEx((*start)->position,(*end)->position,3,RAYWHITE);
    EndTextureMode();

    draw_node(target,**start,GREEN,YELLOW);
    draw_node(target,**end,GREEN,YELLOW);


    *start = NULL;
    *end = NULL;
}

void draw_unconnected_node(RenderTexture2D target) 
{
    Vector2 vect = {
        .x = GetMouseX(),
        .y = GetMouseY()
    };
    Node *node = graph_create_node(&g,vect,RED,g.nodes_pool_size);
    draw_node(target,*node,RED,YELLOW);
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Algo Visualizer");
    SetTargetFPS(240);

    // Create a RenderTexture2D to use as a canvas
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    // Clear render texture before entering the game loop
    BeginTextureMode(target);
    ClearBackground(BACKGROUNDCOLOR);
    EndTextureMode();

    graph_init(&g,MAX_NODES);

    Node *edge_start,*edge_end;
    edge_start = NULL;
    edge_end = NULL;

    //
    // Game loop --------------------------------------------------------------
    while (!WindowShouldClose())
    {
        //
        // Update ---------------------------------------------------------------------
        Vector2 mouse = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Node *clicked_node = check_node_collision_mouse(mouse);
            if(!clicked_node)                                           // Draw unconnected nodes
                draw_unconnected_node(target);
            else {
                if(!edge_start) {                                       // Draw edges
                    edge_start = clicked_node;
                    select_edge_start(target,edge_start);
                }
                else if(edge_start != NULL && edge_end == NULL) 
                    select_edge_end(target,&edge_start,&edge_end,mouse);
            }
        }
    
        // Show graph
        if(IsKeyPressed(KEY_S)) {
            display_graph();
        }

        // ----------------------------------------------------------------------------
        // Draw
        BeginDrawing();
            ClearBackground(BACKGROUNDCOLOR);
            DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
            DrawFPS(15,15);
        EndDrawing();
    }
    graph_destroy(&g);
    CloseWindow();

    return 0;
}
