#pragma once

#include "kbo/logger.h"

typedef struct create_allocator_info_s
{
} create_allocator_t;

typedef struct allocator_s
{
    logger_t *logger;
} allocator_t;

allocator_t *create_allocator(create_allocator_t *create_info);
void destroy_allocator(allocator_t *allocator);

void *allocator_allocate(allocator_t *allocator, size_t size, size_t alignment);
void *allocator_reallocate(allocator_t *allocator, void *original, size_t size, size_t alignment);
void allocator_deallocate(allocator_t *allocator, void *memory);
void allocator_notify_allocation(allocator_t *allocator, size_t size);
void allocator_notify_deallocation(allocator_t *allocator, size_t size);