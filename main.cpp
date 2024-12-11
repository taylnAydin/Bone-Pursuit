#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h> 

// Game state
bool game_situation = true;
bool start_button = false;
bool game_over_music_played = false; // Game Over music played?
float elapsedTime = 0.0f;
int score = 0;

// Window dimensions
const int width = 800;
const int height = 450;

// Player data
Texture2D player_texture;
Vector2 player_pos;
Rectangle player_rect;
float player_velocity = 200.0f; // Pixels per second

// Enemy data
Texture2D enemy_texture;
Vector2 enemy_pos;
Rectangle enemy_rect;
float enemy_velocity_x = 200.0f; // Pixels per second
float enemy_velocity_y = 200.0f; // Pixels per second
int frame_counter = 0;

// Music data
Music background_music;
Music gameover_music;

// Level progression function
void game_levels() {
    if (score > 150) {
        player_velocity = 500.0f;
        enemy_velocity_x = 500.0f;
        enemy_velocity_y = 500.0f;
    } else if (score > 100) {
        player_velocity = 400.0f;
        enemy_velocity_x = 400.0f;
        enemy_velocity_y = 400.0f;
    } else if (score > 50) {
        player_velocity = 300.0f;
        enemy_velocity_x = 300.0f;
        enemy_velocity_y = 300.0f;
    } else if (score > 25) {
        player_velocity = 250.0f;
        enemy_velocity_x = 250.0f;
        enemy_velocity_y = 250.0f;
    }
}

// Player movement function
void player_move(float deltaTime) {
    if (IsKeyDown(KEY_D) && player_pos.x < width - player_texture.width) {
        player_pos.x += player_velocity * deltaTime;
    }
    if (IsKeyDown(KEY_A) && player_pos.x > 0) {
        player_pos.x -= player_velocity * deltaTime;
    }
    if (IsKeyDown(KEY_W) && player_pos.y > 0) {
        player_pos.y -= player_velocity * deltaTime;
    }
    if (IsKeyDown(KEY_S) && player_pos.y < height - player_texture.height) {
        player_pos.y += player_velocity * deltaTime;
    }

    // Update player rectangle
    player_rect.x = player_pos.x;
    player_rect.y = player_pos.y;
}

// Enemy movement function
void enemy_move(float deltaTime) {
    enemy_pos.x += enemy_velocity_x * deltaTime;
    enemy_pos.y += enemy_velocity_y * deltaTime;

    // Screen boundary check
    if (enemy_pos.x <= 0 || enemy_pos.x >= width - enemy_texture.width) {
        enemy_velocity_x = -enemy_velocity_x;
    }
    if (enemy_pos.y <= 0 || enemy_pos.y >= height - enemy_texture.height) {
        enemy_velocity_y = -enemy_velocity_y;
    }

    // Update enemy rectangle
    enemy_rect.x = enemy_pos.x;
    enemy_rect.y = enemy_pos.y;
}

// Restart game function
void restart_game() {
    player_pos = (Vector2){400, 225};
    player_rect = (Rectangle){player_pos.x, player_pos.y, (float)player_texture.width, (float)player_texture.height};

    enemy_pos = (Vector2){200, 225};
    enemy_rect = (Rectangle){enemy_pos.x, enemy_pos.y, (float)enemy_texture.width, (float)enemy_texture.height};

    game_situation = true;
    game_over_music_played = false;
    elapsedTime = 0.0f;
    score = 0;
    player_velocity = 200.0f;
    enemy_velocity_x = 200.0f;
    enemy_velocity_y = 200.0f;

    PlayMusicStream(background_music);
    StopMusicStream(gameover_music);
}

int main() {
    InitWindow(width, height, "BONE PURSUIT");
    InitAudioDevice();
    srand(time(NULL));

    // Load player and enemy textures
    player_texture = LoadTexture("assets/player.png");
    player_pos = (Vector2){400, 225};
    player_rect = (Rectangle){player_pos.x, player_pos.y, (float)player_texture.width, (float)player_texture.height};

    enemy_texture = LoadTexture("assets/enemy.png");
    enemy_pos = (Vector2){200, 225};
    enemy_rect = (Rectangle){enemy_pos.x, enemy_pos.y, (float)enemy_texture.width, (float)enemy_texture.height};

    // Load background and music
    Texture2D background_texture = LoadTexture("assets/background.png");
    background_music = LoadMusicStream("audio/musicmain.mp3");
    gameover_music = LoadMusicStream("audio/musicgameover.mp3");
    PlayMusicStream(background_music);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateMusicStream(background_music);
        UpdateMusicStream(gameover_music);

        if (CheckCollisionRecs(enemy_rect, player_rect)) {
            game_situation = false;
            StopMusicStream(background_music);
            if (!game_over_music_played) {
                PlayMusicStream(gameover_music);
                game_over_music_played = true;
            }
        }

        BeginDrawing();
        ClearBackground(GRAY);

        if (!start_button) {
            DrawText("WELCOME TO BONE PURSUIT", 200, 180, 40, BLACK);
            DrawText("PRESS SPACE TO START", 230, 240, 30, RED);

            if (IsKeyPressed(KEY_SPACE)) start_button = true;
        } else if (game_situation) {
            elapsedTime += deltaTime;
            score = (int)elapsedTime;

            player_move(deltaTime);
            enemy_move(deltaTime);
            game_levels();

            DrawTexture(background_texture, 0, 0, WHITE);
            DrawTexture(player_texture, player_pos.x, player_pos.y, WHITE);
            DrawTexture(enemy_texture, enemy_pos.x, enemy_pos.y, WHITE);
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
        } else {
            DrawTexture(background_texture, 0, 0, WHITE);
            DrawText("GAME OVER", 300, 200, 40, RED);
            DrawText("Press Enter to Restart", 280, 260, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                restart_game();
            }
        }

        EndDrawing();
    }

    UnloadTexture(player_texture);
    UnloadTexture(enemy_texture);
    UnloadTexture(background_texture);
    UnloadMusicStream(background_music);
    UnloadMusicStream(gameover_music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
