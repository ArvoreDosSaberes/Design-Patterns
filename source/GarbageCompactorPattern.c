#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define HEAP_SIZE 1024  // Tamanho total do heap
#define BLOCK_SIZE 32    // Tamanho de cada bloco de memória
#define NUM_BLOCKS (HEAP_SIZE / BLOCK_SIZE)

typedef struct MemoryBlock {
    bool in_use;
    void *ptr;
    size_t size;
} MemoryBlock;

typedef struct Segment {
    MemoryBlock blocks[NUM_BLOCKS];
    uint8_t memory[HEAP_SIZE];
    struct Segment *next;
} Segment;

typedef struct BufferedPtr {
    void **ptr;
} BufferedPtr;

typedef struct GarbageCompactor {
    Segment *activeSegment;
    Segment *inactiveSegment;
} GarbageCompactor;

Segment segment1, segment2;
GarbageCompactor gc;

void init_segments() {
    memset(&segment1, 0, sizeof(Segment));
    memset(&segment2, 0, sizeof(Segment));
    gc.activeSegment = &segment1;
    gc.inactiveSegment = &segment2;
}

void *allocate_memory(size_t size) {
    Segment *seg = gc.activeSegment;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (!seg->blocks[i].in
