#include <stdint.h>  // Tipos de dados padronizados
#include <stdbool.h> // Booleanos

#define SMALL_BLOCK_SIZE  64
#define MEDIUM_BLOCK_SIZE 128
#define LARGE_BLOCK_SIZE  256
#define BLOCKS_PER_POOL  10  // Número de blocos fixos por pilha

typedef enum { SMALL, MEDIUM, LARGE } BlockSize;

// Estrutura do bloco reutilizável
typedef struct Block {
    struct Block* next;
} Block;

// Estrutura da pilha de blocos
typedef struct {
    Block* free_list;
    uint8_t memory_pool[BLOCKS_PER_POOL][LARGE_BLOCK_SIZE]; // Buffer estático máximo
    uint8_t block_size;
} BlockStack;

// Estrutura do gerenciador de pilhas
typedef struct {
    BlockStack small_stack;
    BlockStack medium_stack;
    BlockStack large_stack;
} StackManager;

// Estrutura da fábrica de objetos
typedef struct {
    StackManager* stack_manager;
} ObjectFactory;

// Inicializa uma pilha de blocos (pré-alocação)
void init_stack(BlockStack* stack, uint8_t block_size) {
    stack->block_size = block_size;
    stack->free_list = NULL;

    // Divide o buffer estático em blocos reutilizáveis
    for (int i = 0; i < BLOCKS_PER_POOL; i++) {
        Block* block = (Block*)&stack->memory_pool[i][0];
        block->next = stack->free_list;
        stack->free_list = block;
    }
}

// Aloca um bloco reutilizando a lista livre
void* allocate_block(BlockStack* stack) {
    if (!stack->free_list) {
        return NULL; // Nenhum bloco disponível
    }
    Block* block = stack->free_list;
    stack->free_list = block->next;
    return block;
}

// Libera um bloco de volta para a pilha
void free_block(BlockStack* stack, void* block) {
    if (!block) return;
    ((Block*)block)->next = stack->free_list;
    stack->free_list = (Block*)block;
}

// Inicializa o gerenciador de pilhas
void init_stack_manager(StackManager* manager) {
    init_stack(&manager->small_stack, SMALL_BLOCK_SIZE);
    init_stack(&manager->medium_stack, MEDIUM_BLOCK_SIZE);
    init_stack(&manager->large_stack, LARGE_BLOCK_SIZE);
}

// Aloca um bloco baseado no tamanho especificado
void* allocate_from_manager(StackManager* manager, BlockSize size) {
    switch (size) {
        case SMALL: return allocate_block(&manager->small_stack);
        case MEDIUM: return allocate_block(&manager->medium_stack);
        case LARGE: return allocate_block(&manager->large_stack);
    }
    return NULL;
}

// Libera um bloco baseado no tamanho especificado
void free_to_manager(StackManager* manager, void* block, BlockSize size) {
    switch (size) {
        case SMALL: free_block(&manager->small_stack, block); break;
        case MEDIUM: free_block(&manager->medium_stack, block); break;
        case LARGE: free_block(&manager->large_stack, block); break;
    }
}

// Inicializa a fábrica de objetos
void init_object_factory(ObjectFactory* factory, StackManager* manager) {
    factory->stack_manager = manager;
}

// Cria um objeto utilizando a fábrica
void* create_object(ObjectFactory* factory, BlockSize size) {
    return allocate_from_manager(factory->stack_manager, size);
}

// Libera um objeto utilizando a fábrica
void destroy_object(ObjectFactory* factory, void* object, BlockSize size) {
    free_to_manager(factory->stack_manager, object, size);
}

// Função de exemplo para testar no microcontrolador
void test_memory_allocation() {
    StackManager manager;
    ObjectFactory factory;

    init_stack_manager(&manager);
    init_object_factory(&factory, &manager);

    void* obj1 = create_object(&factory, SMALL);
    void* obj2 = create_object(&factory, MEDIUM);
    void* obj3 = create_object(&factory, LARGE);

    if (obj1 && obj2 && obj3) {
        // Simula LED aceso para indicar sucesso
        // gpio_set_level(LED_GPIO, 1);
    }

    destroy_object(&factory, obj1, SMALL);
    destroy_object(&factory, obj2, MEDIUM);
    destroy_object(&factory, obj3, LARGE);

    // Simula LED apagado para indicar memória liberada
    // gpio_set_level(LED_GPIO, 0);
}

