#include <stdio.h>
#include <stdlib.h>
#include "include/raylib.h"
#include "include/raymath.h"
#include "include/graph.h"

#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500

Graph g = {0};


// Add neighbor to root
void add_neighbor(Node *root, Node *neigbor) 
{
    root->neighbors[root->size] = *neigbor;
    root->size++;
}

// Create edge, using distance between two vectors as it's weight
Edge create_edge(Node *start, Node *end) 
{
    int distance = Vector2Distance(start->position,end->position);
    Edge edge = {
        .start = start->position,
        .end = end->position,
        .node_start = start,
        .node_end = end,
        .weight = distance
    };
    return edge;
}

// Add edge to pool of edges
void add_edge(Edge *edge) 
{
    edges[edges_size] = *edge;
    edges_size++;
}

// Check and returns Node if mouse_pos collides with it, NULL if it doesn't
Node *check_node_collision_mouse(Vector2 mouse_pos) {
    for(int i = 0; i < nodes_size; i++) {
        if(CheckCollisionPointCircle(mouse_pos,nodes[i].position,CIRCLE_RADIUS))
            return &nodes[i];
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
    if(nodes_size == 0) {
        printf("No nodes drawn\n");
        return;
    }
    printf("--------------------- Nodes ---------------------\n");
    for(int i = 0; i < nodes_size; i++) {
        Node node = nodes[i];
        printf("Node [%d] Size = %d -> [",node.id,node.size);
        for(int j = 0; j < node.size; j++) {
            if(j == node.size -1) printf("%d",node.neighbors[j].id);
            else printf("%d,",node.neighbors[j].id);
        }
        printf("]\n");   
    }
    printf("--------------------- Edges ---------------------\n");
    printf("Number of edges: %d\n",edges_size);
    for(int i = 0; i < edges_size; i++) {
        Edge edge = edges[i];
        printf("%d <---> %d (Weight: %d)\n",edge.node_start->id,edge.node_end->id,edge.weight);
    }
}

// Free the heap!
void clean_up() 
{
    for(int i = 0; i < nodes_size; i++) {
        free(nodes[i].neighbors);
    }
}

// Initiate drawing an edge, handles color
void select_edge_start(RenderTexture2D target,Node **start) 
{
    draw_node(target,**start,YELLOW,GREEN);
}

// Create an edge between start and end, also updating the neighbors
// TODO - Refactor this, maybe split up data processing and drawing
void select_edge_end(RenderTexture2D target, Node **start, Node **end, Vector2 mouse_pos) 
{   
    // Check if user clicks the start Node to be the end of the edge
    if(CheckCollisionPointCircle(mouse_pos,(*start)->position,CIRCLE_RADIUS)){
        printf("UH-OH!\n");
        if((*start)->size)      
            draw_node(target,**start,GREEN,YELLOW);
        else                    
            draw_node(target,**start,RED,YELLOW);
        *start = NULL;
        return;
    }

    // Check if user clicks on a node
    *end = check_node_collision_mouse(mouse_pos);
    if(end == NULL) 
        return;

    Edge edge = create_edge(*start,*end);

    add_neighbor(*start,*end);
    add_neighbor(*end,*start);
    add_edge(&edge);

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

    Node *neighbors = malloc(sizeof(Node) * MAX_NODES);
    if(neighbors == NULL) {
        printf("Could not generate neighbors for node\n");
        return;
    }
    Node node = {
        .position = vect,
        .neighbors = neighbors,
        .size = 0,
        .g_cost = 0,
        .h_cost = 0,
        .id = nodes_size
    };
    add_node(&node);
    draw_node(target,node,RED,YELLOW);

    printf("Mouse button clicked at (%d, %d)!\n", GetMouseX(), GetMouseY());

}



int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Algo Visualizer");
    SetTargetFPS(120);

    // Create a RenderTexture2D to use as a canvas
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    // Clear render texture before entering the game loop
    BeginTextureMode(target);
    ClearBackground(BACKGROUNDCOLOR);
    EndTextureMode();

    graph_init(&graph,MAX_NODES);

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
                if(!edge_start) {                                      // Draw edges
                    edge_start = clicked_node;
                    select_edge_start(target,&edge_start);
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
    clean_up();
    CloseWindow();

    return 0;
}
