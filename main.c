#include <stdio.h>
#include <stdlib.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define MAX_NODES 100
#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f


typedef struct Node
{
    Vector2 position;
    struct Node *neighbors;
    int size,id,g_cost,h_cost;            
} Node;

Node nodes[MAX_NODES] = {0};
int nodes_size = 0;

void add_node(Node *node)
{
    nodes[nodes_size] = *node;
    nodes_size++;
}

void add_neighbor(Node *root, Node *neigbor) 
{
    root->neighbors[root->size] = *neigbor;
    root->size++;
}


void draw_node(Node node, Color circle_color, Color text_color) 
{
    char id[4] = {0};
    sprintf(id,"%d",node.id);

    Font default_font = GetFontDefault();
    Vector2 text_size = MeasureTextEx(default_font,id,CIRCLE_RADIUS,5);
    Vector2 scale = Vector2Scale(text_size,.5f);

    DrawCircleV(node.position, CIRCLE_RADIUS, circle_color);
    DrawTextEx(default_font,id,Vector2Subtract(node.position,scale),CIRCLE_RADIUS,5,text_color);
}

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
}

void clean_up() 
{
    for(int i = 0; i < nodes_size; i++) {
        free(nodes[i].neighbors);
    }
}

// Initiate drawing an edge, handles color
void select_edge_start(RenderTexture2D target,Node **start, Vector2 mouse_pos) 
{
   for(int i = 0; i < nodes_size; i++) {
        if(CheckCollisionPointCircle(mouse_pos,nodes[i].position,CIRCLE_RADIUS)) {
            *start = &nodes[i];
            BeginTextureMode(target);
            draw_node(**start,YELLOW,GREEN);
            EndTextureMode();
        }
    } 
}

void select_edge_end(RenderTexture2D target, Node **start, Node **end, Vector2 mouse_pos) 
{   
    if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && CheckCollisionPointCircle(mouse_pos,(*start)->position,CIRCLE_RADIUS)){
        return;
    }
    for(int i = 0; i < nodes_size; i++) {
        if(CheckCollisionPointCircle(mouse_pos,nodes[i].position,CIRCLE_RADIUS)) {
            *end = &nodes[i];
            add_neighbor(*start,*end);
            add_neighbor(*end,*start);

            BeginTextureMode(target);
            DrawLineEx((*start)->position,(*end)->position,3,RAYWHITE);
            draw_node(**start,GREEN,YELLOW);
            draw_node(**end,GREEN,YELLOW);
            EndTextureMode();

            *start = NULL;
            *end = NULL;
        }
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "A* Visualizer");
    SetTargetFPS(120);

    // Create a RenderTexture2D to use as a canvas
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    // Clear render texture before entering the game loop
    BeginTextureMode(target);
    ClearBackground(BACKGROUNDCOLOR);
    EndTextureMode();

    Node *edge_start,*edge_end;
    edge_start = NULL;
    edge_end = NULL;
    while (!WindowShouldClose())
    {
        //
        // Update ---------------------------------------------------------------------
        /* Draw unconnected nodes */
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            Vector2 vect = {
                .x = GetMouseX(),
                .y = GetMouseY()
            };

            Node *neighbors = malloc(sizeof(Node) * MAX_NODES);
            if(neighbors == NULL) {
                printf("Could not generate neighbors for node\n");
                return -1;
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

            BeginTextureMode(target);
            draw_node(node,RED,YELLOW);
            EndTextureMode();

            printf("Mouse button clicked at (%d, %d)!\n", GetMouseX(), GetMouseY());
        }
        /* ---------------------------------------------------------------------------- */
        /* Draw connected nodes */

        Vector2 mouse = GetMousePosition();
        if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) 
        {
            if(edge_start == NULL) 
                select_edge_start(target,&edge_start,mouse);
            else if(edge_start != NULL && edge_end == NULL) 
                select_edge_end(target,&edge_start,&edge_end,mouse);
        }
    
        // Show graph
        if(IsKeyPressed(KEY_S)) {
            display_graph();
        }
        /*-------------------------------------------------------------------------------*/

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
