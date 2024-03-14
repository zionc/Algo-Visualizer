#include "raylib.h"
#include "raymath.h"
#include "graph.h"
#include "game.h"

#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// // Graph entry point
// Graph g = {0};

// typedef enum GameState {
//     DRAWING,
//     ANIMATING,
// } GameState;

// typedef struct AnimateEdges {
//     Edge **edges;               // Path of edges we want to animate
//     int current_index;          // Current path we want to animate
//     float current_scale;        // Current distance we want to draw in each frame
// } AnimateEdges;

// // Current state of the program
// GameState STATE = DRAWING;

// // Check and returns Node if mouse_pos collides with it, NULL if it doesn't
// Node *check_node_collision_mouse(Vector2 mouse_pos) 
// {
//     for(int i = 0; i < g.nodes_pool_size; i++) {
//         Node *node = g.nodes_pool[i];
//         if(CheckCollisionPointCircle(mouse_pos,node->position,CIRCLE_RADIUS))
//             return node;
//     }
//     return NULL;
// }


// // Display edges and nodes
// // TODO - A lot going on here...
// void display_graph() 
// {
//     if(g.nodes_pool_size == 0) {
//         printf("No nodes drawn\n");
//         return;
//     }
//     printf("--------------------- Nodes ---------------------\n");
//     for(int i = 0; i < g.nodes_pool_size; i++) {
//         Node *node = g.nodes_pool[i];
//         printf("Node [%d] Size = %d -> [",node->id,node->adjacent_size);
//         for(int j = 0; j < node->adjacent_size; j++) {
//             if(j == node->adjacent_size -1) printf("%d",node->neighbors[j]->id);
//             else printf("%d,",node->neighbors[j]->id);
//         }
//         printf("]\n");   
//     }
//     printf("--------------------- Edges ---------------------\n");
//     printf("Number of edges: %d\n",g.edges_pool_size);
//     for(int i = 0; i < g.edges_pool_size; i++) {
//         Edge *edge = g.edges_pool[i];
//         printf("%d <---> %d (Weight: %d)\n",edge->node_from->id,edge->node_to->id,edge->weight);
//     }
// }


// void register_new_node() 
// {
//     Vector2 vect = {
//         .x = GetMouseX(),
//         .y = GetMouseY()
//     };
//     graph_create_node(&g,vect,g.nodes_pool_size);
// }

// void register_new_edge(Node *start, Node *end)
// {
//     int distance = Vector2Distance(start->position,end->position);
//     graph_connect_weight(&g,start,end,distance);
//     graph_connect_weight(&g,end,start,distance);
// }

// void draw_nodes()
// {
//     for(int i = 0; i < g.nodes_pool_size; i++) {
//         Node *node = g.nodes_pool[i];
//         NodeState state = node->state;
//         switch (state)
//         {
//         case DRAWING_UNCONNECTED:
//             node->color = RED;
//             break;
//         case DRAWING_CONNECTED:
//             node->color = BLUE;
//             break;
//         case DRAWING_SELECTED:
//             node->color = YELLOW;
//             break;
//         case ANIMATING_START:
//             node->color = GREEN;
//             break;
//         case ANIMATING_END:
//             node->color = GREEN;
//             break;
//         case ANIMATING_UNVISITED:
//             node->color = GRAY;
//             break;
//         case ANIMATING_VISITED:
//             node->color = RED;
//             break;
//         default:
//             break;
//         }
//         DrawCircleV(node->position,CIRCLE_RADIUS,node->color);
//     }
// }

// void draw_edges() 
// {
//     for(int i = 0; i < g.edges_pool_size; i++) {
//         Edge edge = *g.edges_pool[i];
//         DrawLineEx(edge.node_from->position,edge.node_to->position,CIRCLE_RADIUS/7,RAYWHITE);
//     }
// }

// void draw_node_text()
// {
//     for(int i = 0; i < g.nodes_pool_size; i++) {
//         Node node = *g.nodes_pool[i];
//         char id[4] = {0};
//         sprintf(id,"%d",node.id);

//         Font default_font = GetFontDefault();
//         Vector2 text_size = MeasureTextEx(default_font,id,CIRCLE_RADIUS,5);
//         Vector2 scale = Vector2Scale(text_size,.5f);

//         DrawTextEx(default_font,id,Vector2Subtract(node.position,scale),CIRCLE_RADIUS,5,BLACK);
//     }
// }

// void update_edge_weights() 
// {
//     for(int i = 0; i < g.edges_pool_size; i++) {
//         Edge *edge = g.edges_pool[i];
//         int weight = Vector2Distance(edge->node_from->position,edge->node_to->position);
//         edge->weight = weight;
//     }
// }

// Node *dfs_helper(Node *from, Node *to, bool *visited, Edge **edges_visited,int *index) {
//     if(from->id == to->id) {
//         return from;
//     }
//     else {
//         visited[from->id] = true;
//         Node *node = NULL;
//         for(int i = 0; i < from->adjacent_size; i++) {
//             Node *neighbor = from->neighbors[i];
//             int neighbor_index = neighbor->id;
//             if(visited[neighbor_index] != true) {
//                 for(int j = 0; j < g.edges_pool_size; j++) {
//                     if(g.edges_pool[j]->node_from == from && g.edges_pool[j]->node_to == neighbor)
//                         edges_visited[*index] = g.edges_pool[j];
//                 }
//                 *index = *index + 1;
//                 node = dfs_helper(neighbor,to,visited,edges_visited,index);
//             }
//             if(node == to) break;
//         }
//         return node;
//     }
// }

// Edge **search_dfs(Node *from, Node *to) {
//     Edge **edges_visited = calloc(g.edges_pool_size,sizeof(Edge*));
//     bool *visited        = malloc(sizeof(bool) * g.nodes_pool_size);
//     if(edges_visited == NULL || visited == NULL) {
//         printf("search_dfs: Failed to malloc\n");
//         exit(1);
//     }
//     int edges_index = 0;
//     Node *node = dfs_helper(from,to,visited,edges_visited,&edges_index);
//     free(visited);
//     if(node == NULL)
//         printf("Could not find Node %d from Node %d\n",from->id,to->id);
//     else
//         printf("Path found from Node %d to Node %d\n",from->id,to->id);
//     return edges_visited;
// }


// void animate_dfs_edges(AnimateEdges *edges_animation) 
// {
//     // Draw paths we've already processed
//     for(int i = 0; i < edges_animation->current_index; i++) {
//         Edge *edge = edges_animation->edges[i];
//         DrawLineEx(edge->node_from->position,edge->node_to->position,CIRCLE_RADIUS/3.5f,YELLOW);
//     }

//     // If there are no more edges to animate, return
//     if(edges_animation->edges[edges_animation->current_index] == 0) {
//         return;
//     }

//     // Animate current edge
//     Edge *edge  = edges_animation->edges[edges_animation->current_index]; 
//     float scale = edges_animation->current_scale;
//     Vector2 draw_to_vector = Vector2MoveTowards(edge->node_from->position, edge->node_to->position,scale);
//     DrawLineEx(edge->node_from->position,draw_to_vector,CIRCLE_RADIUS/3.5f,YELLOW);

//     // How fast we want to animate
//     edges_animation->current_scale+=2.5f;

//     // Move on to next edge
//     if(edges_animation->current_scale >= edge->weight) {
//         edges_animation->current_index++;
//         edges_animation->current_scale = 0.f;
//         if(edge->node_to->state != ANIMATING_END)
//             edge->node_to->state = ANIMATING_VISITED;
//     }
// }


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Algo Visualizer");
    SetTargetFPS(120);
    
    Graph graph = {0};
    Game game = {0};
    graph_init(&graph,MAX_NODES);
    game_init(&game,&graph,10);

    
    // Node *edge_start = NULL;
    // bool state_Moving_Nodes = false;
    // Node *clicked_node = NULL;
    // Node *start = NULL;
    // Node *end = NULL;
    // AnimateEdges *animate_edges = malloc(sizeof(AnimateEdges));
    // if(animate_edges == NULL) {
    //     printf("main: Could not malloc edges for animation\n");
    //     exit(1);
    // }
    
    // Game loop  ---------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        
        // Update ---------------------------------------------------------------------

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            game_update_left_click(&game);

        // // Drag logic
        // if(state_Moving_Nodes && clicked_node) {
        //     clicked_node->position = GetMousePosition();
        //     if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        //         state_Moving_Nodes = false;
        //         update_edge_weights();
        //     }
        // }

        // // Set start/end of path
        // if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        //     Node *node = check_node_collision_mouse(GetMousePosition());
        //     if(node != NULL && start == NULL) {
        //         start = node;
        //         start->state = ANIMATING_START;
        //         printf("Start --> %d\n",start->id);
        //     }
        //     else if(node != NULL && start != NULL) {
        //         end   = node;
        //         printf("End --> %d\n", end->id);
        //         if(animate_edges->edges!= NULL) free(animate_edges->edges);
        //         Edge **paths = search_dfs(&g,start,end);
        //         animate_edges->edges = paths;
        //         animate_edges->current_index = 0;
        //         animate_edges->current_scale = 0.f;

        //         // Gray out nodes
        //         for(int i = 0; i < g.nodes_pool_size; i++) {
        //             if(g.nodes_pool[i] != start)
        //                 g.nodes_pool[i]->state = ANIMATING_UNVISITED;
        //         }
        //         end->state   = ANIMATING_END;
        //         STATE = ANIMATING;
        //         start = NULL;
        //         end   = NULL;
        //     }
        //     else start = NULL;
        // }


        // // Show graph stdout
        // if(IsKeyPressed(KEY_S)) {
        //     display_graph();
        // }

        
        // ----------------------------------------------------------------------------
        // Draw

        BeginDrawing();
            ClearBackground(BACKGROUNDCOLOR);
            game_draw_edges(&game);
            game_draw_nodes(&game);
            game_draw_node_id(&game);
            DrawFPS(15,15);
        EndDrawing();
    }
    graph_destroy(&graph);
    CloseWindow();

    return 0;
}
