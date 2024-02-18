#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "graph.h"
#include <string.h>
#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500


// Graph entry point
Graph g = {0};

typedef enum GameState {
    DRAWING,
    ANIMATING,
} GameState;

typedef struct AnimateEdges {
    Edge **edges;               // Path of edges we want to animate
    int current_index;          // Current path we want to animate
    float current_scale;        // Current distance we want to draw in each frame
} AnimateEdges;

// Current state of the program
GameState STATE = DRAWING;

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
            if(j == node->adjacent_size -1) printf("%d",node->neighbors[j]->id);
            else printf("%d,",node->neighbors[j]->id);
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

Node *dfs_helper(Node *from, Node *to, bool *visited, Edge **edges_visited,int *index) {
    if(from->id == to->id) {
        return from;
    }
    else {
        visited[from->id] = true;
        Node *node = NULL;
        for(int i = 0; i < from->adjacent_size; i++) {
            Node *neighbor = from->neighbors[i];
            int neighbor_index = neighbor->id;
            if(visited[neighbor_index] != true) {
                for(int j = 0; j < g.edges_pool_size; j++) {
                    if(g.edges_pool[j]->node_from == from && g.edges_pool[j]->node_to == neighbor)
                        edges_visited[*index] = g.edges_pool[j];
                }
                *index = *index + 1;
                node = dfs_helper(neighbor,to,visited,edges_visited,index);
            }
            if(node == to) break;
        }
        return node;
    }
}

Edge **search_dfs(Node *from, Node *to) {
    Edge **edges_visited = calloc(g.edges_pool_size,sizeof(Edge*));
    bool *visited        = malloc(sizeof(bool) * g.nodes_pool_size);
    int edges_index = 0;
    Node *node = dfs_helper(from,to,visited,edges_visited,&edges_index);
    free(visited);
    if(node == NULL)
        printf("Could not find Node %d from Node %d\n",from->id,to->id);
    else
        printf("Path found from Node %d to Node %d\n",from->id,to->id);
    return edges_visited;
}


void animate_dfs_edges(AnimateEdges *edges_animation) 
{
    // Draw paths we've already processed
    for(int i = 0; i < edges_animation->current_index; i++) {
        Edge *edge = edges_animation->edges[i];
        DrawLineEx(edge->node_from->position,edge->node_to->position,CIRCLE_RADIUS/4,YELLOW);
    }

    if(edges_animation->edges[edges_animation->current_index] == 0) {
        return;
    }

    // Animate current edge
    Edge *edge  = edges_animation->edges[edges_animation->current_index]; 
    float scale = edges_animation->current_scale;
    Vector2 draw_to_vector = Vector2MoveTowards(edge->node_from->position, edge->node_to->position,scale);
    DrawLineEx(edge->node_from->position,draw_to_vector,CIRCLE_RADIUS/4,YELLOW);
    edges_animation->current_scale+=2;
    if(edges_animation->current_scale >= edge->weight) {
        edges_animation->current_index++;
        edges_animation->current_scale = 0.f;
    }
}


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Algo Visualizer");
    SetTargetFPS(120);

    graph_init(&g,MAX_NODES);

    Node *edge_start = NULL;
    bool state_Moving_Nodes = false;
    Node *clicked_node = NULL;
    Node *start = NULL;
    Node *end = NULL;
    AnimateEdges *animate_edges = malloc(sizeof(AnimateEdges));
    
    // Game loop  ---------------------------------------------------------------------
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
                update_edge_weights();
            }
        }

        // Set start/end of path
        if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            Node *node = check_node_collision_mouse(GetMousePosition());
            if(node != NULL && start == NULL) {
                start = node;
                printf("Start --> %d\n",start->id);
            }
            else if(node != NULL && start != NULL) {
                end   = node;
                printf("End --> %d\n", end->id);
                if(animate_edges->edges!= NULL) free(animate_edges->edges);
                Edge **paths = search_dfs(start,end);
                animate_edges->edges = paths;
                animate_edges->current_index = 0;
                animate_edges->current_scale = 0.f;
                STATE = ANIMATING;
                start = NULL;
                end   = NULL;
            }
        }


        // Show graph stdout
        if(IsKeyPressed(KEY_S)) {
            display_graph();
        }

        // 
        // ----------------------------------------------------------------------------
        // Draw

        // TODO: This is terrible
        BeginDrawing();
            ClearBackground(BACKGROUNDCOLOR);
            draw_edges();
            if(STATE == ANIMATING) animate_dfs_edges(animate_edges);
            draw_nodes();
            draw_node_text();
            DrawFPS(15,15);
        EndDrawing();
    }
    graph_destroy(&g);
    if(animate_edges->edges) free(animate_edges->edges);
    free(animate_edges);
    CloseWindow();

    return 0;
}
