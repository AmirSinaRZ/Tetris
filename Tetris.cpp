/*
    11/05/2024 AmirSinaRZ
    IAU Student Project
    1403/08/15
*/



#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;
const int BLOCK_SIZE = 30;

SDL_Color blockColors[7] = {
    {0, 255, 255, 255}, // I - Cyan
    {255, 255, 0, 255}, // O - Yellow
    {128, 0, 128, 255}, // T - Purple
    {255, 165, 0, 255}, // L - Orange
    {0, 0, 255, 255},   // J - Blue
    {0, 255, 0, 255},   // S - Green
    {255, 0, 0, 255}    // Z - Red
};

class Tetris {
public:
    Tetris() : window(nullptr), renderer(nullptr), score(0), gameOver(false) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
            cerr << "Could not initialize SDL or SDL_ttf: " << SDL_GetError() << endl;
            exit(1);
        }
        
        window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                  WIDTH * BLOCK_SIZE, HEIGHT * BLOCK_SIZE, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            cerr << "Could not create window: " << SDL_GetError() << endl;
            exit(1);
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            cerr << "Could not create renderer: " << SDL_GetError() << endl;
            exit(1);
        }

        board.resize(HEIGHT, vector<int>(WIDTH, 0));
        generatePiece();

        currentX = WIDTH / 2 - 1;
        currentY = 0;
        font = TTF_OpenFont("arial.ttf", 24);
        if (font == nullptr) {
            cerr << "Could not open font: " << TTF_GetError() << endl;
            exit(1);
        }
    }

    ~Tetris() {
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }

    void run() {
        bool quit = false;
        SDL_Event e;
        
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            if (canMove(currentX - 1, currentY)) currentX--;
                            break;
                        case SDLK_RIGHT:
                            if (canMove(currentX + 1, currentY)) currentX++;
                            break;
                        case SDLK_DOWN:
                            dropPiece();
                            break;
                        case SDLK_UP:
                            rotatePiece();
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                }
            }

            if (!gameOver) {
                dropPiece();
            }
            draw();
            SDL_Delay(300);
        }
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    vector<vector<int>> board;
    vector<vector<int>> currentPiece;
    int currentX, currentY;
    int currentColorIndex;
    int score;
    bool gameOver;

    void generatePiece() {
        vector<vector<vector<int>>> shapes = {
            {{1, 1, 1, 1}}, // I
            {{1, 1}, {1, 1}}, // O
            {{0, 1, 0}, {1, 1, 1}}, // T
            {{1, 0, 0}, {1, 1, 1}}, // L
            {{0, 0, 1}, {1, 1, 1}}, // J
            {{1, 1, 0}, {0, 1, 1}}, // S
            {{0, 1, 1}, {1, 1, 0}}  // Z
        };
        int pieceIndex = rand() % 7;
        currentPiece = shapes[pieceIndex];
        currentColorIndex = pieceIndex;
    }

    void draw() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the grid
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        for (int i = 0; i <= HEIGHT; ++i) {
            SDL_RenderDrawLine(renderer, 0, i * BLOCK_SIZE, WIDTH * BLOCK_SIZE, i * BLOCK_SIZE);
        }
        for (int j = 0; j <= WIDTH; ++j) {
            SDL_RenderDrawLine(renderer, j * BLOCK_SIZE, 0, j * BLOCK_SIZE, HEIGHT * BLOCK_SIZE);
        }

        // Draw the board
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH; ++j) {
                if (board[i][j]) drawBlock(j, i, blockColors[board[i][j] - 1]);
            }
        }

        // Draw the current piece
        for (int i = 0; i < currentPiece.size(); ++i) {
            for (int j = 0; j < currentPiece[i].size(); ++j) {
                if (currentPiece[i][j]) {
                    drawBlock(currentX + j, currentY + i, blockColors[currentColorIndex]);
                }
            }
        }

        // Draw score
        renderText("Score: " + to_string(score), 10, 10, {255, 255, 255, 255});

        // Draw game over message
        if (gameOver) {
            renderText("Game Over! Press Esc to exit.", WIDTH * BLOCK_SIZE / 2 - 150, HEIGHT * BLOCK_SIZE / 2, {255, 0, 0, 255});
        }

        SDL_RenderPresent(renderer);
    }

    void renderText(const string& message, int x, int y, SDL_Color color) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void drawBlock(int x, int y, SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect block = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &block);
    }

    bool canMove(int newX, int newY) {
        for (int i = 0; i < currentPiece.size(); ++i) {
            for (int j = 0; j < currentPiece[0].size(); ++j) {
                if (currentPiece[i][j]) {
                    if (newX + j < 0 || newX + j >= WIDTH || newY + i >= HEIGHT || board[newY + i][newX + j]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void dropPiece() {
        if (!canMove(currentX, currentY + 1)) {
            placePiece();
            clearLines();
            generatePiece();
            currentX = WIDTH / 2 - 1;
            currentY = 0;
            if (!canMove(currentX, currentY)) {
                gameOver = true; // Set game over flag
            }
        } else {
            currentY++;
        }
    }

    void placePiece() {
        for (int i = 0; i < currentPiece.size(); ++i) {
            for (int j = 0; j < currentPiece[0].size(); ++j) {
                if (currentPiece[i][j])
                    board[currentY + i][currentX + j] = currentColorIndex + 1;
            }
        }
        score += 10; // Increase score when piece is placed
    }

    void clearLines() {
        for (int i = HEIGHT - 1; i >= 0; --i) {
            if (isLineComplete(i)) {
                board.erase(board.begin() + i);
                board.insert(board.begin(), vector<int>(WIDTH, 0));
                score += 100; // Increase score for each line cleared (H)
            }
        }
    }

    bool isLineComplete(int line) {
        for (int j = 0; j < WIDTH; ++j) {
            if (board[line][j] == 0) return false;
        }
        return true;
    }

    void rotatePiece() {
        vector<vector<int>> rotated(currentPiece[0].size(), vector<int>(currentPiece.size()));
        for (int i = 0; i < currentPiece.size(); ++i) {
            for (int j = 0; j < currentPiece[0].size(); ++j) {
                rotated[j][currentPiece.size() - 1 - i] = currentPiece[i][j];
            }
        }
        if (canMove(currentX, currentY)) {
            currentPiece = rotated;
        }
    }
};

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));
    Tetris game;
    game.run();
    return 0;
}
