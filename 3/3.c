#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SUCCESS 1
#define FAIL 11

int max_thread_count = 0;

pthread_mutex_t mutex;

typedef struct thread_data {
        int K;
        int current_tour;
        int first_player_score;
        int second_player_score;
        int experiments_count;

        int* first_won;
        int current_experiment;
} thread_data;

void CheckBounds(int to_check, int lower_bound) {
        if (to_check < lower_bound) {
                printf("The value must be greater or equal to %d\n", lower_bound);
                exit(-1);
        }
}

thread_data ScanData() {
        thread_data data;

        printf("Enter thread count: ");
        scanf("%d", &max_thread_count);
        printf("\n");
        CheckBounds(max_thread_count, 1);

        printf("Enter the number of throws: ");
        scanf("%d", &data.K);
        printf("\n");
        CheckBounds(data.K, 1);

        printf("Enter the tour number: ");
        scanf("%d", &data.current_tour);
        printf("\n");
        CheckBounds(data.current_tour, 1);

        printf("Enter first and second player's score: ");
        scanf("%d", &data.first_player_score);
        scanf("%d", &data.second_player_score);
        printf("\n");
        CheckBounds(data.first_player_score, 2);
        CheckBounds(data.second_player_score, 2);

        printf("Enter count of experiments to conduct: ");
        scanf("%d", &data.experiments_count);
        printf("\n");
        CheckBounds(data.experiments_count, 1);

        int arr[data.experiments_count];
        data.first_won = arr;
        data.current_experiment = 0;

        return data;
}

void* conduct_experiment(void* args) {
        thread_data* data = (thread_data*) args;
        pthread_mutex_lock(&mutex);
        int first = data->first_player_score;
        int second = data->second_player_score;
        for (int i = data->current_tour; i <= data->K; ++i) {
                first += rand() % 12;
                second += rand() % 12;
        }
        int is_won = (first > second) ? 1 : 0;
        data->first_won[data->current_experiment++] = is_won;
        pthread_mutex_unlock(&mutex);
        return (void*) SUCCESS;
}

int main() {
        thread_data data = ScanData();

        pthread_t threads[max_thread_count];
        int status_addrs[max_thread_count];
        int current_thread = 0;
        int current_thread_count = max_thread_count;
        for (int i = 0; i < data.experiments_count; ++i) {
                if (current_thread_count > 0) {
                        if (pthread_create(&threads[current_thread], NULL, conduct_experiment, (void*) &data) == 0) {
                                current_thread_count--;
                                pthread_join(threads[current_thread], (void **) &status_addrs[current_thread]);
                                if (status_addrs[current_thread] != SUCCESS) {
                                        printf("ERROR\n");
                                        exit(-1);
                                }
                        }
                        current_thread++;
                } else {
                        current_thread = 0;
                        current_thread_count = max_thread_count;
                }
        }

        int first_won_count = 0;
        for (int i = 0; i < data.experiments_count; ++i) {
                if (data.first_won[i] == 1) {
                        first_won_count++;
                }
        }

        double first_chance = (1.0 * first_won_count) / (1.0 * data.experiments_count);
        double second_chance = 1.0 - first_chance;
        printf("First player's chances to win: %f\n", first_chance);
        printf("Second player's chances to win: %f\n", second_chance);

        return 0;
}