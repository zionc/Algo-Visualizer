#include "raylib.h"
#include "raymath.h"
#include "graph.h"
#include "game.h"

#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 20.f
#define MAX_NODES 500
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Algo Visualizer");
    SetTargetFPS(120);
    
    Graph graph = {0};
    Game game = {0};
    graph_init(&graph,MAX_NODES);
    game_init(&game,&graph,10);
    
    // Game loop  ---------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        
        // Update ---------------------------------------------------------------------

        if(IsKeyPressed(KEY_D)) {
            if(game.state == DRAWING) game.state = DRAGGING;
            else                      game.state = DRAWING;
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && game.state == DRAWING)
            game_update_left_click(&game);
        else if(game.state == DRAGGING)
            game_update_left_drag(&game);


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
