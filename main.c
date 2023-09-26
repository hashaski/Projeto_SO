/*
Integrantes
    - André Matteucci - 32273541
    - Enzo Koji - 32273754
    - Felipe Ribeiro - 32212720
*/


#include <pthread.h> // usada para criar uma thread para cada filósofo, passando o seu identificador como parâmetro, e para esperar que todas as threads terminem.
#include <semaphore.h> //  usada para criar um semáforo mutex para controlar o acesso à função de pegar ou soltar os garfos, e um vetor de semáforos para controlar o comportamento de cada filósofo.
#include <stdio.h> // usada para imprimir mensagens indicando o estado e as ações dos filósofos
#include <unistd.h> // usada para fornecer a função sleep, que faz a thread esperar por um tempo determinado em segundos.

#define NUM_PHILOSOPHERS 5 // número de filósofos
#define THINKING 2 // estado de pensando
#define HUNGRY 1 // estado de com fome
#define EATING 0 // estado de comendo
#define LEFT_NEIGHBOR ((philosopher_id + 4) % NUM_PHILOSOPHERS) // índice do vizinho à esquerda
#define RIGHT_NEIGHBOR ((philosopher_id + 1) % NUM_PHILOSOPHERS) // índice do vizinho à direita

int philosopher_state[NUM_PHILOSOPHERS]; // vetor que guarda o estado de cada filósofo
int philosopher_id[NUM_PHILOSOPHERS] = { 0, 1, 2, 3, 4 }; // vetor que guarda o id de cada filósofo

sem_t fork_mutex; // semáforo mutex para controlar o acesso à função de pegar ou soltar os garfos
sem_t philosopher_semaphore[NUM_PHILOSOPHERS]; // vetor de semáforos para controlar o comportamento de cada filósofo

void check_if_can_eat(int philosopher_id) // função que verifica se o filósofo pode comer
{
    if (philosopher_state[philosopher_id] == HUNGRY && philosopher_state[LEFT_NEIGHBOR] != EATING && philosopher_state[RIGHT_NEIGHBOR] != EATING) // se o filósofo está com fome e seus vizinhos não estão comendo
    {
        philosopher_state[philosopher_id] = EATING; // muda o estado do filósofo para comendo

        sleep(2); // espera um tempo

        printf("Filosofo %d pega o garfo %d e %d\n", philosopher_id + 1, LEFT_NEIGHBOR + 1, philosopher_id + 1); // imprime uma mensagem indicando que o filósofo pegou os dois garfos
        printf("Filosofo %d esta comendo\n", philosopher_id + 1); // imprime uma mensagem indicando que o filósofo está comendo

        sem_post(&philosopher_semaphore[philosopher_id]); // libera o semáforo do filósofo
    }
}

void try_to_get_forks(int philosopher_id) // função que tenta pegar os garfos
{
    sem_wait(&fork_mutex); // espera pelo semáforo mutex

    philosopher_state[philosopher_id] = HUNGRY; // muda o estado do filósofo para com fome

    printf("Filosofo %d esta com fome\n", philosopher_id + 1); // imprime uma mensagem indicando que o filósofo está com fome

    check_if_can_eat(philosopher_id); // chama a função check_if_can_eat para verificar se o filósofo pode comer

    sem_post(&fork_mutex); // libera o semáforo mutex

    sem_wait(&philosopher_semaphore[philosopher_id]); // espera pelo semáforo do filósofo

    sleep(1); // espera um tempo
}

void release_forks(int philosopher_id) // função que solta os garfos
{
    sem_wait(&fork_mutex); // espera pelo semáforo mutex

    philosopher_state[philosopher_id] = THINKING; // muda o estado do filósofo para pensando

    printf("Filosofo %d solta o garfo %d e %d\n", philosopher_id + 1, LEFT_NEIGHBOR + 1, philosopher_id + 1); // imprime uma mensagem indicando que o filósofo soltou os dois garfos
    printf("Filosofo %d esta pensando\n", philosopher_id + 1); // imprime uma mensagem indicando que o filósofo está pensando

    check_if_can_eat(LEFT_NEIGHBOR); // chama a função check_if_can_eat para verificar se o vizinho à esquerda pode comer
    check_if_can_eat(RIGHT_NEIGHBOR); // chama a função check_if_can_eat para verificar se o vizinho à direita pode comer

    sem_post(&fork_mutex); // libera o semáforo mutex
}

void* act_as_philosopher(void* num) // função que representa cada filósofo
{
    while (1) // repete indefinidamente
    {
        int* id = num; // obtém o id do filósofo

        sleep(1); // espera um tempo

        try_to_get_forks(*id); // chama a função try_to_get_forks para tentar pegar os garfos

        sleep(0); // espera um tempo

        release_forks(*id); // chama a função release_forks para soltar os garfos
    }
}

int main() // função principal
{
    int i; // variável auxiliar
    pthread_t thread_id[NUM_PHILOSOPHERS]; // vetor de threads para representar cada filósofo

    sem_init(&fork_mutex, 0, 1); // inicializa o semáforo mutex com valor 1

    for (i = 0; i < NUM_PHILOSOPHERS; i++) // para cada filósofo
        sem_init(&philosopher_semaphore[i], 0, 0); // inicializa o semáforo do filósofo com valor 0

    for (i = 0; i < NUM_PHILOSOPHERS; i++) // para cada filósofo
    {
        pthread_create(&thread_id[i], NULL, act_as_philosopher, &philosopher_id[i]); // cria uma thread para representar o filósofo e passa o seu id como parâmetro
        printf("Filosofo %d esta pensando\n", i + 1); // imprime uma mensagem indicando que o filósofo está pensando
    }

    for (i = 0; i < NUM_PHILOSOPHERS; i++) // para cada filósofo
        pthread_join(thread_id[i], NULL); // espera pela thread do filósofo terminar
}
