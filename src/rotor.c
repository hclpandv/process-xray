#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define WIDTH 32
#define HEIGHT 16
#define TOP 5
#define LEFT 25
#define DELAY 150000

volatile sig_atomic_t running = 1;

void stop(int sig)
{
    (void)sig;
    running = 0;
}

int main(void)
{
    int pid = getpid();
    int active = 0;
    int perimeter = 2 * (WIDTH + HEIGHT - 2);

    signal(SIGINT, stop);
    signal(SIGTERM, stop);

    printf("\033[2J\033[?25l");

    while (running) {

        for (int y = 0; y < HEIGHT; y++) {
            printf("\033[%d;%dH", TOP + y, LEFT);

            for (int x = 0; x < WIDTH; x++) {

                int pos = -1;

                if (y == 0)
                    pos = x;
                else if (x == WIDTH - 1)
                    pos = WIDTH - 1 + y;
                else if (y == HEIGHT - 1)
                    pos = WIDTH + HEIGHT - 2 + (WIDTH - 1 - x);
                else if (x == 0)
                    pos = 2 * WIDTH + HEIGHT - 3 + (HEIGHT - 1 - y);

                if (pos >= 0 && pos == active)
                    printf("\033[1;93m●\033[0m");
                else if (pos >= 0)
                    printf("\033[2;37m·\033[0m");
                else
                    printf(" ");
            }
        }

        printf("\033[%d;%dHPID: %d",
               TOP + HEIGHT / 2,
               LEFT + WIDTH / 2 - 5,
               pid);

        fflush(stdout);

        active = (active + 1) % perimeter;
        usleep(DELAY);
    }

    printf("\033[0m\033[2J\033[H\033[?25h");

    return 0;
}