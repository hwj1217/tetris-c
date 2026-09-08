#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "tetris.h"

void place_piece(GameState *state) {
    for(int i = 0; i < 4; i++) {
        int draw_row = state -> current.base.row + state -> all_pieces[state -> current.type].rotations[state -> current.rotIndex][i].row;
        int draw_col = state -> current.base.col + state -> all_pieces[state -> current.type].rotations[state -> current.rotIndex][i].col;
        state -> board[draw_row][draw_col] = state -> current.type;
    }
}

void clear_piece(GameState *state) {
    for(int i = 0; i < 4; i++) {
        int clear_row = state -> current.base.row + state -> all_pieces[state -> current.type].rotations[state -> current.rotIndex][i].row;
        int clear_col = state -> current.base.col + state -> all_pieces[state -> current.type].rotations[state -> current.rotIndex][i].col;
        state -> board[clear_row][clear_col] = EMPTY;
    }
}

int is_valid_position(GameState *state, int new_base_row, int new_base_col, int new_rot_index) {
    for(int i = 0; i < 4; i++) {
        int row = new_base_row + state -> all_pieces[state -> current.type].rotations[new_rot_index][i].row;
        int col = new_base_col + state -> all_pieces[state -> current.type].rotations[new_rot_index][i].col;

        if((col < 0 || col >= 10 || row >= 20) || state -> board[row][col] != EMPTY) {
            return 0;
        }
    }
    return 1;
}

int clear_lines(GameState *state) {
    int lines_cleared = 0;
    int row = 19;
    while(row >= 0) {
        int isFull = 1;
        for(int col = 0; col < 10; col++) {
            if(state -> board[row][col] == EMPTY) {
                isFull = 0;
            }
        }
        if(isFull) {
            for(int r = row; r > 0; r--) {
                for(int col = 0; col < 10; col++) {
                    state -> board[r][col] = state -> board[r - 1][col];
                }
            }
            for(int col = 0; col < 10; col++) {
                state -> board[0][col] = EMPTY;
            }
            lines_cleared += 1;
        }
        else{
            row--;
        }
    }
    return lines_cleared;
}

int main(void) {

    srand(time(NULL));

    GameState game = {
        .all_pieces[PIECE_O] = {
            PIECE_O,
            {
                { {0, 0}, {0, 1}, {1, 0}, {1, 1} },
                { {0, 0}, {0, 1}, {1, 0}, {1, 1} },
                { {0, 0}, {0, 1}, {1, 0}, {1, 1} },
                { {0, 0}, {0, 1}, {1, 0}, {1, 1} }
            },
            'O'
        },

        .all_pieces[PIECE_I] = {
            PIECE_I,
            {
                { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
                { {1, 0}, {1, 1}, {1, 2}, {1, 3} },
                { {0, 1}, {1, 1}, {2, 1}, {3, 1} },
                { {1, 0}, {1, 1}, {1, 2}, {1, 3} }
            },
            'I'
        },

        .all_pieces[PIECE_T] = {
            PIECE_T,
            {
                { {0, 1}, {1, 0}, {1, 1}, {1, 2} },
                { {0, 1}, {1, 1}, {1, 2}, {2, 1} },
                { {1, 0}, {1, 1}, {1, 2}, {2, 1} },
                { {0, 1}, {1, 0}, {1, 1}, {2, 1} }
            },
            'T'
        },

        .all_pieces[PIECE_S] = {
            PIECE_S,
            {
                { {0, 1}, {0, 2}, {1, 0}, {1, 1} },
                { {0, 0}, {1, 0}, {1, 1}, {2, 1} },
                { {0, 1}, {0, 2}, {1, 0}, {1, 1} },
                { {0, 0}, {1, 0}, {1, 1}, {2, 1} }
            },
            'S'
        },

        .all_pieces[PIECE_Z] = {
            PIECE_Z,
            {
                { {0, 0}, {0, 1}, {1, 1}, {1, 2} },
                { {0, 1}, {1, 1}, {1, 0}, {2, 0} },
                { {0, 0}, {0, 1}, {1, 1}, {1, 2} },
                { {0, 1}, {1, 1}, {1, 0}, {2, 0} }
            },
            'Z'
        },

        .all_pieces[PIECE_J] = {
            PIECE_J,
            {
                { {0, 0}, {1, 0}, {1, 1}, {1, 2} },
                { {0, 0}, {0, 1}, {1, 0}, {2, 0} },
                { {0, 0}, {0, 1}, {0, 2}, {1, 2} },
                { {0, 1}, {1, 1}, {2, 0}, {2, 1} }
            },
            'J'
        },

        .all_pieces[PIECE_L] = {
            PIECE_L,
            {
                { {0, 2}, {1, 0}, {1, 1}, {1, 2} },
                { {0, 0}, {1, 0}, {2, 0}, {2, 1} },
                { {0, 0}, {0, 1}, {0, 2}, {1, 0} },
                { {0, 0}, {0, 1}, {1, 1}, {2, 1} }
            },
            'L'
        },

        .current = {
            rand() % 7 + 1,
            {5, 5},
            0
        }
    };

    place_piece(&game);

    // 1. 初始化 ncurses
    initscr();      // 進入 ncurses 模式
    cbreak();       // 關閉行緩衝，按鍵能立刻被讀到，不用等 enter
    noecho();       //按鍵不要自動顯示在畫面上
    keypad(stdscr, TRUE);    //讓方向鍵之類的特殊鍵能被讀到
    curs_set(0);    //隱藏游標
    timeout(500);    //設定 getch 最多等待 500 毫秒

    start_color();
    init_pair(PIECE_L, COLOR_RED, COLOR_BLACK);
    init_pair(PIECE_O, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PIECE_S, COLOR_WHITE, COLOR_BLACK);
    init_pair(PIECE_Z, COLOR_BLUE, COLOR_BLACK);
    init_pair(PIECE_I, COLOR_GREEN, COLOR_BLACK);
    init_pair(PIECE_J, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(PIECE_T, COLOR_RED, COLOR_BLACK);

    // 定義遊戲畫面的邊框範圍
    int board_width = 10;
    int board_height = 20;

    while (1) {

        clear();

        for(int row = 0; row <= board_height + 1; row++) {
            for(int col = 0; col <= board_width + 1; col++) {
                if(row == 0 || row == board_height + 1 || col == 0 || col == board_width + 1) {
                    mvaddch(row, col, '#');    // 在 （row, col）印出 '#'
                }
            }
        }

        for(int row = 0; row < board_height; row++) {
            for(int col = 0; col < board_width; col++) {
                if(game.board[row][col] != EMPTY) {
                    mvaddch(row + 1, col + 1, '#' | COLOR_PAIR(game.board[row][col]));
                }
            }
        }

        mvprintw(2, 14, "Score: %d", game.score);

        refresh();    // 把剛剛畫的內容真正輸出到畫面上

        // 等待按鍵
        int key = getch();

        //處理按鍵
        if(key == ERR) {
            clear_piece(&game);
            if(is_valid_position(&game, game.current.base.row + 1, game.current.base.col, game.current.rotIndex)) {
                game.current.base.row += 1;
                place_piece(&game);
            }
            else {
                place_piece(&game);
                game.score += clear_lines(&game) * 100;
                game.current = (Fall){.type = rand() % 7 + 1, .base = {5, 5}, .rotIndex = 0};
                if(is_valid_position(&game, 5, 5, 0) == 0) {
                    mvprintw(4, 14, "GAME OVER!");
                    refresh();
                    napms(2000);
                    break;
                }
                place_piece(&game);
            }
        }
        else if(key == 'q') {
            break;
        }
        else if(key == KEY_UP) {
            clear_piece(&game);
            int new_rot = (game.current.rotIndex + 1) % 4;
            if(is_valid_position(&game, game.current.base.row, game.current.base.col, new_rot)) {
                game.current.rotIndex = new_rot;
            }
            place_piece(&game);
        }
        else if(key == KEY_LEFT) {
            clear_piece(&game);
            if(is_valid_position(&game, game.current.base.row, game.current.base.col - 1, game.current.rotIndex)) {
                game.current.base.col -= 1;
            }
            place_piece(&game);
        }
        else if(key == KEY_RIGHT) {
            clear_piece(&game);
            if(is_valid_position(&game, game.current.base.row, game.current.base.col + 1, game.current.rotIndex)) {
                game.current.base.col += 1;
            }
            place_piece(&game);
        }
        else if(key == KEY_DOWN) {
            clear_piece(&game);
            if(is_valid_position(&game, game.current.base.row + 1, game.current.base.col, game.current.rotIndex)) {
                game.current.base.row += 1;
            }
            place_piece(&game);
        }
    }

    endwin();   // 離開 ncurses 模式，恢復正常終端機

    return 0;
}
