#include "mazeEnv.h"
#include "SARSA.h"
#include "functions.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>


//Initialise Q.
void MakeQ(){
   Q = malloc(rows*cols*sizeof(float*));
   //printf("Première malloc ok");

    for(int i=0; i<rows*cols; i++){
        Q[i] = malloc(4*sizeof(float));
        for(int j=0; j<4; j++){
            Q[i][j] = 1;
        }
    }
}

void freeQ(){
    for(int i=0; i<rows*cols; i++){
        free(Q[i]);
    }
    free(Q);
}


//Lit dans Q.
float Qread(int row, int col, action a){
    return(Q[row*cols + col][a]);
}

//Affiche Q.
void Qrender(){
    for (int i=0; i<rows*cols; i++) {
        for (int j=0; j< number_actions; j++){
             printf("%f ", Q[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


//Choix d'une action avec la politique epsilon-greedy.
action eps_greedy(){
    action act = env_action_sample();
    
    //Choix aléatoire
    if (rand()%101>eps*100){
        float rew = Q[state_row*cols + state_col][act];
        for (int k=0; k<4; k++){
            if (Q[state_row*cols + state_col][k] > rew){
                rew = Q[state_row*cols + state_col][k];
                act = k;
            }
        }
    }
    return(act);
}


//Fonction qui renvoie 1 si la case actuelle est un mur, 0 sinon.
int iswall(){
    int* current = &visited[state_row][state_col];
    if(*current == wall){
        return(1);
    }
    else{
        return(0);
    }
}


//Une boucle d'entraînement du programme.
void training (){
    //Re-génération des points visités
    init_visited();

    //Re-génération d'une clé pour l'aléatoire.
    srand(time(NULL));
    
    //Remise à zéro du labyrinthe
    maze_reset();
    
    //On compte les pas effectués pour sortir du labyrinthe.
    int step = 0;

    //Errance jusqu'à l'arrivée.
    int sortie = 0;
        
    //Choix d'une action
        action a1 = eps_greedy();
        
        
    while(sortie == 0){
    
        //Mémorisation de la position de départ.
        int old_row = state_row;
        int old_col = state_col;
        
        //déplacement
        envOutput state = maze_step(a1);
        
        //Lecture de la récompense et ajustement si on rentre dans un mur.
        float reward = state.reward;
        int prev_row = state.new_row;
        int prev_col = state.new_col;
        int w = iswall();
        if (w == 1){
            reward = -50;
        }
        else {
            reward = -1;
        }
        
        //Choix d'une action
        action a2 = eps_greedy();
        
        //Application de la formule.
        Q[old_row*cols + old_col][a1] = Q[old_row*cols + old_col][a1] + alp*(reward + gam*Q[prev_row*cols + prev_col][a2] - Q[old_row*cols + old_col][a1]);

        //Si on rentre dans un mur annulation du déplacement.
        if (w ==1){
            state_col = old_col;
            state_row = old_row;
        }
        
        //choix de l'action a2
        a1 = a2;
        
        //On ajoute une miette si on a besoin de visualiser la trajectoire pas par pas pour débugguer.
        int* current = &visited[state_row][state_col];
        *current = crumb;

        //On vérifie si on est sur la sortie.
        if ((state_row == goal_row)&&(state_col == goal_col)){
            sortie = 1;
        }
        step+=1;
    }
    printf("sortie trouvée en %d pas\n",step);
}

int main()
{
    maze_make("maze.txt");
    init_visited();
    MakeQ();
    
    Qrender();
    maze_render();
    
    for(int i = 0; i<iter; i++){
        printf("itération %d avec epsilon = %f\n",i, eps);
        eps = -((eps_beginning)/iter)*i + eps_beginning;
        training();
        printf("fin de l'itération %d\n",i);
    }

    //Dernier passage sans aléatoire.
    eps=0;
    training();
    
    //Qrender();
    add_crumbs();
    maze_render();
    freeQ();
    
    return 0;
}
