#include <stdio.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define MAX_NODES 100
#define BACKGROUNDCOLOR BLACK
#define CIRCLE_RADIUS 15.f

typedef struct Node
{
    Vector2 position;
    struct Node *neighbors;
} Node;

Vector2 circles[MAX_NODES] = {0};

void add_circle(Vector2 vect, int node_index)
{
    circles[node_index] = vect;
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "A* Visualizer");
    SetTargetFPS(60);

    // Create a RenderTexture2D to use as a canvas
    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);

    // Clear render texture before entering the game loop
    BeginTextureMode(target);
    ClearBackground(BACKGROUNDCOLOR);
    EndTextureMode();

    int node_index = 0;
    bool isDrawingEdge = false;
    while (!WindowShouldClose())
    {
        //
        // Update ---------------------------------------------------------------------
        /* Draw unconnected nodes */
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            Vector2 vect = {
                .x = GetMouseX(),
                .y = GetMouseY()};

            add_circle(vect, node_index);
            BeginTextureMode(target);
            DrawCircleV(vect, CIRCLE_RADIUS, RED);
            EndTextureMode();
            node_index++;
            printf("Mouse button clicked at (%d, %d)!\n", GetMouseX(), GetMouseY());
        }
        /* ---------------------------------------------------------------------------- */
        /* Connect circles */
        Vector2 mouse = GetMousePosition();
        Vector2 start,end;

        if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && !isDrawingEdge) 
        {
            for(int i = 0; i < node_index; i++) {
                if(CheckCollisionPointCircle(mouse,circles[i],CIRCLE_RADIUS)) {
                    start = circles[i];
                    BeginTextureMode(target);
                    DrawCircleV(start,CIRCLE_RADIUS,YELLOW);
                    isDrawingEdge = true;
                    EndTextureMode(); 
                }
            }
        }

        if(isDrawingEdge)
        {
            if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && !CheckCollisionPointCircle(mouse,start,30.f)) {
                for(int i = 0; i < node_index; i++) {
                    if(CheckCollisionPointCircle(mouse,circles[i],CIRCLE_RADIUS)) {
                        end = circles[i];
                        BeginTextureMode(target);
                        DrawLineV(start,end,RAYWHITE);
                        DrawCircleV(end,CIRCLE_RADIUS,GREEN);
                        DrawCircleV(start,CIRCLE_RADIUS,GREEN);
                        isDrawingEdge = false;
                        EndTextureMode();
                    }
                }
            }
        }
        /*-------------------------------------------------------------------------------*/
        // Draw
        BeginDrawing();

        ClearBackground(BACKGROUNDCOLOR);
        DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
