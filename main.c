#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

void print_at_xy(int x, int y, char *val);
void display_score();
void init();
int zero_lives();
void set_game_state_over();
char get_input();
void update_player(char);
void update_wall();
void increment_score();
void decrement_lives();
void draw();
void draw_wall();
void draw_ninja();
void clean_up();
void clear_screen();
void display_message(const char *, int yOffset);
void update_ninja(char ch);
int collides_with_spike();
void display_count_down(); 

HANDLE _output_handle;

void hidecursor()
{
   _output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;
   info.bVisible = FALSE;
   SetConsoleCursorInfo(_output_handle, &info);
}

const int SCREEN_WIDTH = 12;
const int SCREEN_HEIGHT = 20;
int lives;
int game_state;
int GAME_STATE_OVER;
int GAME_STATE_PLAYING;
int GOAL_POINTS;
int WALL_SPEED;
int score;
char avatar[2];
char game_over_string[30];
char left_wall[60];
char right_wall[60];
char left_spike[3];
char right_spike[3];
char ninja[2];
int wall_y_pos;
int ninja_x;
int ninja_y;
int NINJA_SPEED;
int ninja_delta;
int left_wall_spike;
int right_wall_spike;
int immunity_count_down;

int main(){

    init();
    printf("\ninit");
    system("@cls||clear");

    //1000/30
    //game loop
    while(1){

        if(immunity_count_down > 0){
            immunity_count_down--;
        }

        clear_screen();    

        char ch = get_input();

        //clear screen and quit
        if(game_state == GAME_STATE_OVER && ch == 'q'){
            system("@cls||clear");
            break;
        }

        clear_screen();
        update_wall();
        update_ninja(ch);
        draw();
        if(collides_with_spike() && immunity_count_down <= 0){
            decrement_lives();
            immunity_count_down = 30;
        }
        
        if(zero_lives()){
            set_game_state_over();
            display_message(game_over_string, -2);
            display_message("'q' to quit...", 0);
        }
        Sleep(100);

    }

    clean_up();
}

void init(){
    score = 0;
    lives = 3;
    GOAL_POINTS = 10;
    GAME_STATE_OVER = 1;
    GAME_STATE_PLAYING = 2;
    WALL_SPEED = 1;

    ninja_x = 1;
    ninja_y = SCREEN_HEIGHT/2;
    NINJA_SPEED = 6;
    ninja_delta = 0;
    left_wall_spike = 0;
    right_wall_spike = 0;
    immunity_count_down = 30;

    game_state = GAME_STATE_PLAYING;
    wall_y_pos = -20;
    strcpy(left_spike, "|>");
    strcpy(right_spike, "<|");
    strcpy(game_over_string, "GAME OVER");
    strcpy(ninja, "X");

    strcpy(left_wall,  "|||>|||||||||||||>>||||||>>||||||>>||||");
    strcpy(right_wall, "|||||||||<||||<||||||||<||||||<<||||||<");
    hidecursor();
}

int zero_lives(){
    if(lives == 0){
        return 1;
    }
    return 0;
}

void set_game_state_over(){
    game_state = GAME_STATE_OVER;
}

char get_input(){
    char ch = 0;

    if(kbhit()){
        ch = getch();
    }

    return ch;
}

void update_player(char ch){

}

void increment_score(){
    score += GOAL_POINTS;
}

void decrement_lives(){
    lives--;
}

void draw(){
    draw_wall();
    draw_ninja();
    display_score();
    display_count_down();    
}

void print_at_xy(int x, int y, char *val)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(_output_handle, coord);
  printf("%s", (const char *)val);
  fflush(stdout);
}

void display_score(){
    char buffer[50] = {0};
    sprintf(buffer, "SCORE: %4d LIVES: %d", score, lives);
    print_at_xy(0, 0, buffer);
}

void clear_screen(){
    char buffer[] = "            ";

    for(int i=0;i<3;i++)
    {
        print_at_xy(0, i, buffer);
    }
}

void display_message(const char *message, int yOffset){
    char buffer[100] = {0};
    strcpy(buffer, message);
    print_at_xy(SCREEN_WIDTH/2 - strlen(message)/2, 
                (SCREEN_HEIGHT/2 - 1)+yOffset, buffer);
}

void display_count_down(){
    if(immunity_count_down > 0){
        char buffer[3] = {0};
        char *countdown = itoa(immunity_count_down/10, buffer, 10);
        strcpy(buffer, countdown);
        SetConsoleTextAttribute (_output_handle, FOREGROUND_BLUE);
        display_message("GET READY!", -2);
        display_message(buffer, 0);
        SetConsoleTextAttribute (_output_handle, FOREGROUND_INTENSITY);
    }
}

void clean_up(){
    printf("Thanks for playing.");
}

void update_wall(){
    wall_y_pos += WALL_SPEED;
    if(wall_y_pos > 0){
        wall_y_pos = -SCREEN_HEIGHT;
    }
}

void update_ninja(char ch){
    ninja_x += ninja_delta;
    if(ninja_x == 1 && ch == 'j' && game_state == GAME_STATE_PLAYING){
        ninja_delta = NINJA_SPEED;
        ninja_x += ninja_delta;
        increment_score();
    }
    else if(ninja_x == SCREEN_WIDTH-1 && ch == 'j' && game_state == GAME_STATE_PLAYING){
        ninja_delta = -NINJA_SPEED;
        ninja_x += ninja_delta;
        increment_score();
    }
    else if(ninja_x <= 1){
        ninja_delta = 0;
        ninja_x = 1;        
    }
    else if(ninja_x >= SCREEN_WIDTH-1){
        ninja_delta = 0;
        ninja_x = SCREEN_WIDTH-1;
    }  

    if(immunity_count_down > 10 && lives < 3){
        ninja_x = SCREEN_WIDTH/2;
        ninja_y += 1;
        if(ninja_y >= SCREEN_HEIGHT){
            ninja_y = SCREEN_HEIGHT;
        }
    }
    if(immunity_count_down < 10 && immunity_count_down > 1){
        ninja_x = 1;
        ninja_y = SCREEN_HEIGHT / 2;
    }  
}

int collides_with_spike(){
    if(game_state == GAME_STATE_OVER){
        return 0;
    }

    if(ninja_x == 1 && left_wall_spike == 1){
        return 1;
    }
    else if(ninja_x == SCREEN_WIDTH-1 && right_wall_spike == 1){
        return 1;
    }

    return 0;
}

void draw_wall(){
    char wall_row[SCREEN_WIDTH+1];
    int wall_index = wall_y_pos * -1;
    left_wall_spike = 0;
    right_wall_spike = 0;
    for(int i=2;i<20;i++,wall_index++){

        for(int j=1;j<SCREEN_WIDTH;j++){
            wall_row[j] = ' ';
        }

        wall_row[SCREEN_WIDTH+1] = '\0';

        wall_row[0] = '|';
        wall_row[SCREEN_WIDTH] = '|';
        
        if(left_wall[wall_index] == '>'){            
            wall_row[1] = '>';
            if(i==SCREEN_HEIGHT/2){
                left_wall_spike = 1;
            }
        }


        if(right_wall[wall_index] == '<'){            
            wall_row[SCREEN_WIDTH-1] = '<';
            if(i==SCREEN_HEIGHT/2){
                right_wall_spike = 1;
            }
        }

        print_at_xy(0, i, wall_row);
    }
}

void draw_ninja(){

    if(ninja_y >= SCREEN_HEIGHT) return;

    SetConsoleTextAttribute (_output_handle, FOREGROUND_RED);
    print_at_xy(ninja_x, ninja_y, ninja); 
    SetConsoleTextAttribute (_output_handle, FOREGROUND_INTENSITY);  
}