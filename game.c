#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define DELAY 30000

typedef enum _ends {
    PlayerWin = 0,
    EnemyWin = 1
} Ends;

typedef struct _player {
    int y;
    int x;
    int max_y;
    int next_y;
    int direction;
} Player;

Ends play_game();

int main(int argc, char *argv[]) {
    // Set up

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    Ends end;
    int c;

    while (1) { // Game loop
        end = play_game();

        if (end == PlayerWin) {
            mvprintw(2, 2, "Player wins!");
        } else {
            mvprintw(2, 2, "Enemy wins....");
        }

        nodelay(stdscr, FALSE);
        
        mvprintw(4, 2, "Continue? Press enter.");

        c = getch();

        if (c == 10) {
            continue;
        } else {
            break;
        }
    }

    endwin();
}

Ends play_game() {
    // Setting/Resetting variables
    nodelay(stdscr, TRUE);

    Player user, enemy;
    int ball_x = 5, ball_y = 5;
    int max_x = 0, max_y = 0;
    int next_x = 0;
    int next_y = 0;
    int direction_x = 1;
    int direction_y = 1;
    int score = 0, enemy_score = 0;
    int c, r;
    int half_screen;
    int score_debounce = 0;

    getmaxyx(stdscr, max_y, max_x);

    user.max_y = max_y;
    user.y = 1;
    user.x = 1;
    enemy.max_y = max_y;
    enemy.y = max_y - 1;
    enemy.x = max_x - 1;
    user.direction = 0;
    enemy.direction = -1;
    half_screen = ((int) (max_y / 2));

    while (1) {
        // New screen size updating
        getmaxyx(stdscr, max_y, max_x);
        user.max_y = max_y;
        enemy.max_y = max_y;

        // Score debounce
        if (score_debounce >= 1) {
            score_debounce++;
            if (score_debounce >= 8) {
                score_debounce = 0;
            }
        }

        // Clearing and redrawing
        clear();
        // Ball
        mvprintw(ball_y, ball_x, "O");
        // Enemy paddle
        mvprintw(enemy.y, max_x - 2, "|");
        mvprintw(enemy.y - 1, max_x - 2, "|");
        mvprintw(enemy.y + 1, max_x - 2, "|");
        // Player paddle
        mvprintw(user.y, 1, "|");
        mvprintw(user.y - 1, 1, "|");
        mvprintw(user.y + 1, 1, "|");
        // Nets
        mvprintw(half_screen + 3, 0, "_");
        mvprintw(half_screen - 3, 0, "_");
        mvprintw(half_screen + 3, max_x - 1, "_");
        mvprintw(half_screen - 3, max_x - 1, "_");
        // Scores
        mvprintw(0, 4, "P: %d", score);
        mvprintw(0, max_x - 6, "E: %d", enemy_score);
        refresh();

        usleep(DELAY);

        next_x = ball_x + direction_x;
        next_y = ball_y + direction_y;
        enemy.next_y = enemy.y + enemy.direction;

        // Ball x movement

        if (next_x >= max_x - 1 || next_x < 0) {
            direction_x *= -1;
        } else {
            ball_x += direction_x;
        }

        // Ball y movement

        if (next_y >= max_y || next_y < 0) {
            direction_y *= -1;
        } else {
            ball_y += direction_y;
        }

        // Enemy control

        r = rand() % 11;

        switch (r) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5: // Follow ^
                if (enemy.y > ball_y) {
                    enemy.y -= 1;
                } else if (enemy.y < ball_y) {
                    enemy.y += 1;
                } else {
                    enemy.y += 0;
                }
                break;
            case 6:
            case 7: // Up
                if (enemy.y - 1 > 0) {
                    enemy.y -= 1;
                }
                break;
            case 8:
            case 9: // Down
                if (enemy.y + 1 <= max_y) {
                    enemy.y += 1;
                }
                break;
            case 10: // Nothing
                break;
        }

        // Move control

        c = getch();

        switch (c) {
            case KEY_UP:
                if (user.y > 0) {
                    user.y--;
                }
                break;
            case KEY_DOWN:
                if (user.y <= max_y) {
                    user.y++;
                }
                break;
        }

        // Paddle-Ball impact + Game end

        // Player contact
        if (ball_x == 1) {
            if (ball_y == user.y + 1) {
                direction_x = 1;
                direction_y = 1;
            } else if (ball_y == user.y - 1) {
                direction_x = 1;
                direction_x = -1;
            } else if (ball_y == user.y) {
                direction = 1;
            }
        }

        // Player scoring
        if (score_debounce == 0 && ball_x == max_x - 2) {
            if (ball_y >= half_screen - 3 && ball_y <= half_screen + 3) {
                score++;
                score_debounce = 1;

                if (score >= 10) {
                    return PlayerWin;
                }
            }
        }

        // Enemy contact
        if (ball_x >= max_x - 3) {
            if (ball_y == enemy.y + 1) {
                direction_x = -1;
                direction_y = 1;
            } else if (ball_y == enemy.y - 1) {
                direction_x = -1;
                direction_x = -1;
            } else if (ball_y == enemy.y) {
                direction_x = -1;
            }
        }

        // Enemy scoring
        if (score_debounce == 0 && ball_x == 0) {
            if (ball_y >= half_screen - 3 && ball_y <= half_screen + 3) {
                enemy_score++;
                score_debounce = 1;

                if (enemy_score >= 10) {
                    return EnemyWin;
                }
            }
        }
    }
}
