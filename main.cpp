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
int width = 800;
int height = 450;

// Player data
Texture2D player_texture;
Vector2 player_pos;
Rectangle player_rect;
float player_velocity = 200.0f; // Pixel per second

// Enemy data
Texture2D enemy_texture;
Vector2 enemy_pos;
Rectangle enemy_rect;
float enemy_velocity_x = 200.0f; // Pixel per second
float enemy_velocity_y = 200.0f; // Pixel per second
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
    if (IsKeyDown(KEY_D)) {
        if (player_pos.x < width - player_texture.width) {
            player_pos.x += player_velocity * deltaTime;
        }
    }
    if (IsKeyDown(KEY_A)) {
        if (player_pos.x > 0) {
            player_pos.x -= player_velocity * deltaTime;
        }
    }
    if (IsKeyDown(KEY_W)) {
        if (player_pos.y > 0) {
            player_pos.y -= player_velocity * deltaTime;
        }
    }
    if (IsKeyDown(KEY_S)) {
        if (player_pos.y < height - player_texture.height) {
            player_pos.y += player_velocity * deltaTime;
        }
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
    if (enemy_pos.x <= 0) {
        enemy_pos.x = 0;
        enemy_velocity_x = fabs(enemy_velocity_x); // Move right
    }
    if (enemy_pos.x >= width - enemy_texture.width) {
        enemy_pos.x = width - enemy_texture.width;
        enemy_velocity_x = -fabs(enemy_velocity_x); // Move left
    }
    if (enemy_pos.y <= 0) {
        enemy_pos.y = 0;
        enemy_velocity_y = fabs(enemy_velocity_y); // Move down
    }
    if (enemy_pos.y >= height - enemy_texture.height) {
        enemy_pos.y = height - enemy_texture.height;
        enemy_velocity_y = -fabs(enemy_velocity_y); // Move up
    }

    // Random direction change
    frame_counter++;
    if (frame_counter > 60) { // Change direction every second (at 60 FPS)
        frame_counter = 0;
        int random_direction = rand() % 4;
        switch (random_direction) {
            case 0: enemy_velocity_x = fabs(enemy_velocity_x); break; // Move right
            case 1: enemy_velocity_x = -fabs(enemy_velocity_x); break; // Move left
            case 2: enemy_velocity_y = fabs(enemy_velocity_y); break; // Move down
            case 3: enemy_velocity_y = -fabs(enemy_velocity_y); break; // Move up
        }
    }

    // Update enemy rectangle
    enemy_rect.x = enemy_pos.x;
    enemy_rect.y = enemy_pos.y;
}

// Restart game function
void restart_game() {
    // Restart player and enemy
    player_pos = (Vector2){400, 225};
    player_rect = (Rectangle){player_pos.x, player_pos.y, (float)player_texture.width, (float)player_texture.height};

    enemy_pos = (Vector2){200, 225};
    enemy_rect = (Rectangle){enemy_pos.x, enemy_pos.y, (float)enemy_texture.width, (float)enemy_texture.height};

    // Reset game state and music
    game_situation = true;
    game_over_music_played = false;
    PlayMusicStream(background_music);
    StopMusicStream(gameover_music);

    // Reset score and time
    elapsedTime = 0.0f;
    score = 0;
    enemy_velocity_x = 200.0f;
    enemy_velocity_y = 200.0f;
    player_velocity = 200.0f;
}

int main() {
    // Initialize window
    InitWindow(width, height, "BONE PURSUIT");

    // Initialize audio device
    InitAudioDevice();

    // Initialize randomness
    srand(time(NULL));

    // Load player and enemy
    player_texture = LoadTexture("player.png");
    player_pos = (Vector2){400, 225};
    player_rect = (Rectangle){player_pos.x, player_pos.y, (float)player_texture.width, (float)player_texture.height};

    enemy_texture = LoadTexture("enemy.png");
    enemy_pos = (Vector2){200, 225};
    enemy_rect = (Rectangle){enemy_pos.x, enemy_pos.y, (float)enemy_texture.width, (float)enemy_texture.height};

    // Load background and icon
    Texture2D background_texture = LoadTexture("background.png");
    Image icon = LoadImage("icon.png");
    if (icon.data != NULL) {
        SetWindowIcon(icon);
        UnloadImage(icon);
        TraceLog(LOG_INFO, "Icon loaded and set successfully.");
    } else {
        TraceLog(LOG_ERROR, "Failed to load icon image.");
    }

    // Load music
    background_music = LoadMusicStream("musicmain.mp3");
    gameover_music = LoadMusicStream("musicgameover.mp3");
    PlayMusicStream(background_music);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        // Calculate deltaTime
        float deltaTime = GetFrameTime();
        
        // Update music
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
            DrawText("WELCOME TO BONE PURSUIT", width / 20 + 80, height / 2 - 80, 40, BLACK);
            DrawText("PRESS SPACE TO START", width / 20 + 100, height / 2, 40, RED);
            DrawText("USE W A S D TO ESCAPE FROM THE SKULL!!!", width / 20 + 180, height / 2 + 50, 15, BLACK);
            DrawText("MADE BY TAYLAN KEMAL AYDIN", width / 20 - 20, height - 30, 15, WHITE);

            if (IsKeyDown(KEY_SPACE)) {
                start_button = true;
            }
        }

        if (game_situation && start_button) {
            // Update elapsed time and score
            elapsedTime += deltaTime;
            score = (int)elapsedTime;

            // Update player movement
            player_move(deltaTime);
            enemy_move(deltaTime);

            // Draw textures
            DrawTexture(background_texture, -115, 0, WHITE);
            DrawTexture(player_texture, player_pos.x, player_pos.y, WHITE);
            DrawTexture(enemy_texture, enemy_pos.x, enemy_pos.y, WHITE);

            // Update game levels
            if (score % 25 == 0 && score >= 25) {
                game_levels();
            }

            // Display score
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
        } else if (start_button) {
            DrawTexture(background_texture, -115, 0, WHITE);
            DrawTexture(player_texture, player_pos.x, player_pos.y, WHITE);
            DrawTexture(enemy_texture, enemy_pos.x, enemy_pos.y, WHITE);
            
            const char* gameOverText = "GAME OVER";
            int textWidth = MeasureText(gameOverText, 40);
            DrawText(gameOverText, (width - textWidth) / 2, height / 2 - 20, 40, RED);

            const char* restartText = "Press Enter to Restart";
            int restartTextWidth = MeasureText(restartText, 20);
            DrawText(restartText, (width - restartTextWidth) / 2, height / 2 + 30, 20, WHITE);

                    // Display final score
            DrawText(TextFormat("Final Score: %d", score), (width - MeasureText(TextFormat("Final Score: %d", score), 20)) / 2, height / 2 + 60, 20, WHITE);

            // Wait for enter key to restart the game
            if (IsKeyPressed(KEY_ENTER)) {
                restart_game();
            }
        }

        EndDrawing();
    }

        // Unload resources
        UnloadTexture(player_texture);
        UnloadTexture(enemy_texture);
        UnloadTexture(background_texture);

        // Unload music
        UnloadMusicStream(background_music);
        UnloadMusicStream(gameover_music);

        // Close audio device
        CloseAudioDevice();

        // Close window
        CloseWindow();

        return 0;
}