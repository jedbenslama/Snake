#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

char getch(void) { // fonction getch car elle n'est pas sur mac (source: chatgpt)
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void print_area(int listex[100], int listey[100], int largeurliste, int pomme_x, int pomme_y) {
    for (int i = 0; i < 18; i++) { // definir la zone en carré (18x18)
        for (int j = 0; j < 18; j++) {
            if (i == 0 || j == 0 || j == 18 - 1 || i == 18 - 1) {
                printf("# "); // un espace pour que la zone soit carrée
            } else {
                int snake = 0; // variable pour checker si une partie de la grille fait partie du serpent ou pas
                for (int k = 0; k < largeurliste; k++) {
                    if (i == listey[k] && j == listex[k]) { // si serpent alors print "O "
                        printf("O ");
                        snake = 1;
                        break;
                    }
                }
                if (!snake) {
                    if (i == pomme_y && j == pomme_x) { // pareil mais pour la pomme
                        printf("o ");
                    } else {
                        printf("  "); // si ni serpent ni pomme alors la zone est vide on print un espace
                    }
                }
            }
        }
        printf("\n");
    }
}

void update_snake(int listex[100], int listey[100], int *tailleserpent, char direction, int *pomme_x, int *pomme_y) { // je met toujours 100 car ce sera la taille max du serpent
    int ex_x = listex[0];
    int ex_y = listey[0];
    int temp_x, temp_y;

    if (direction == 'w' | direction == 'z') listey[0]--; // pour que ça marche sur azerty et querty
    else if (direction == 's') listey[0]++;
    else if (direction == 'a' | direction == 'q') listex[0]--;
    else if (direction == 'd') listex[0]++;

    if (listex[0] == *pomme_x && listey[0] == *pomme_y) {
        (*tailleserpent)++;
        *pomme_x = 1 + rand() % (18 - 2);
        *pomme_y = 1 + rand() % (18 - 2);
    }

    for (int i = 1; i < *tailleserpent; i++) { // pour que les bouts du serpent se suivent
        temp_x = listex[i]; 
        temp_y = listey[i];
        listex[i] = ex_x;
        listey[i] = ex_y;
        ex_x = temp_x;
        ex_y = temp_y;
    }
}

int check_collision(int listex[100], int listey[100], int tailleserpent) {
    if (listex[0] <= 0 || listex[0] >= 18 - 1 || listey[0] <= 0 || listey[0] >= 18 - 1) { // checker si la tete du serpent a dépassé la grille (inutile de verifier le reste du corps car c'est la tete qui dirige)
        return 1;
    }

    for (int i = 1; i < tailleserpent; i++) {
        if (listex[0] == listex[i] && listey[0] == listey[i]) { // checker si je me suis mordu la queue
            return 1;
        }
    }

    return 0;
}

int kbhit(void) { // fonction kbhit car elle n'est pas sur mac (source: chatgpt)
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main() {
    int listex[100];
    int listey[100];
    
    listex[0]=5;
    // listex[1]=4; // "décommenter" pour que le serpent apparaisse en entier, laisser commenter pour une apparition un peu plus originale
    // listex[2]=3;
    listey[0]=5;
    // listey[1]=5;
    // listey[2]=5;
    
    
    int tailleserpent = 3; // taille de départ

    char direction = 'd'; // direction de depart

    int pomme_x =1 + rand() % (18 - 2); // lieu d'apparition de la premiere pomme, elle peut spawn sur le serpent donc elle apparaitra quand le serpent a quitté sa zone
    int pomme_y =1 + rand() % (18 - 2);

    srand(time(NULL));

    while (1) {
        printf("\e[1;1H\e[2J"); // effacer la console, source https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c

        print_area(listex, listey, tailleserpent, pomme_x, pomme_y);

        update_snake(listex, listey, &tailleserpent, direction, &pomme_x, &pomme_y);

        if (check_collision(listex, listey, tailleserpent)) { // game over si y a collision avec mur ou avec soi meme
            printf("Game Over!");
            break;
        }

        usleep(300000); // 300000 microsecondes soit 0.3 sec

        if (kbhit()) {// checker si une touche est hit et si la touche est z/q/s/d/a/w
            char new_direction = getch();
            if ((new_direction == 'w' && direction != 's') || // pour pas faire demi-tour et donc mourir instantanément car y aura collision
                (new_direction == 's' && direction != 'w') ||
                (new_direction == 'a' && direction != 'd') ||
                (new_direction == 'd' && direction != 'a')) {
                direction = new_direction;
            }
        }
    }

    return 0;
}
