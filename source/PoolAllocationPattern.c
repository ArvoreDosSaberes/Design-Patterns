#include <stdio.h>
#include <stdint.h>

#define POOL_SIZE 1000 // Número máximo de objetos TempData

// Estrutura de dados de temperatura
typedef struct {
    int id;
    float temperature;
} TempData;

// Estrutura para o Pool de Recursos
typedef struct {
    TempData pool[POOL_SIZE];  // Array de objetos
    uint8_t allocated[POOL_SIZE];  // Flags para indicar quais objetos estão em uso
} TempDataPool;

// Inicializa o pool de dados
void initPool(TempDataPool* pool) {
    for (int i = 0; i < POOL_SIZE; i++) {
        pool->allocated[i] = 0;  // Nenhum objeto está alocado inicialmente
    }
}

// Aloca um objeto TempData do pool
TempData* allocateTempData(TempDataPool* pool) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool->allocated[i]) {  // Se não estiver em uso
            pool->allocated[i] = 1; // Marca como alocado
            return &pool->pool[i];
        }
    }
    return NULL; // Pool esgotado
}

// Libera um objeto TempData e o devolve ao pool
void releaseTempData(TempDataPool* pool, TempData* data) {
    int index = data - pool->pool;  // Calcula o índice do objeto
    if (index >= 0 && index < POOL_SIZE) {
        pool->allocated[index] = 0; // Marca como livre
    }
}

// Simulação de sensores e histórico
void simulateUsage() {
    TempDataPool tempPool;
    initPool(&tempPool);

    TempData* temp1 = allocateTempData(&tempPool);
    if (temp1) {
        temp1->id = 1;
        temp1->temperature = 23.5;
        printf("TempData %d alocado: %.2f°C\n", temp1->id, temp1->temperature);
    }

    TempData* temp2 = allocateTempData(&tempPool);
    if (temp2) {
        temp2->id = 2;
        temp2->temperature = 24.0;
        printf("TempData %d alocado: %.2f°C\n", temp2->id, temp2->temperature);
    }

    releaseTempData(&tempPool, temp1);
    printf("TempData %d liberado\n", temp1->id);

    TempData* temp3 = allocateTempData(&tempPool);
    if (temp3) {
        temp3->id = 3;
        temp3->temperature = 22.8;
        printf("TempData %d alocado: %.2f°C\n", temp3->id, temp3->temperature);
    }
}

int main() {
    simulateUsage();
    return 0;
}
