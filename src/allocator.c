#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "kbo/allocator.h"

typedef struct allocation_header_s
{
    void *base;
    size_t size;
} allocation_header_t;

static size_t normalize_alignment(size_t alignment)
{
    if (alignment < sizeof(void *))
    {
        return sizeof(void *);
    }
    return alignment;
}

static allocation_header_t *get_allocation_header(void *memory)
{
    return (allocation_header_t *)((unsigned char *)memory - sizeof(allocation_header_t));
}

allocator_t *create_allocator(create_allocator_t *create_info)
{
    allocator_t *allocator = (allocator_t *)malloc(sizeof(allocator_t));
    if (allocator == NULL)
    {
        return NULL;
    }
    allocator->logger = create_logger(&(create_logger_info_t){
        .name = "allocator",
    });
    logger_log(allocator->logger, LOGGER_SEVERITY_INFO, "Creating allocator");
    return allocator;
}

void destroy_allocator(allocator_t *allocator)
{
    logger_log(allocator->logger, LOGGER_SEVERITY_INFO, "Destroying allocator");
    destroy_logger(allocator->logger);
    free(allocator);
}

void *allocator_allocate(allocator_t *allocator, size_t size, size_t alignment)
{
    size_t effective_alignment = normalize_alignment(alignment);
    size_t metadata_size = sizeof(allocation_header_t);
    size_t total_size = 0;
    void *base = NULL;
    uintptr_t aligned_address = 0;
    allocation_header_t *header = NULL;

    if ((effective_alignment & (effective_alignment - 1)) != 0)
    {
        return NULL;
    }

    if (size > SIZE_MAX - metadata_size - (effective_alignment - 1))
    {
        return NULL;
    }

    total_size = size + metadata_size + (effective_alignment - 1);
    base = malloc(total_size);
    if (base == NULL)
    {
        return NULL;
    }

    aligned_address = ((uintptr_t)base + metadata_size + (effective_alignment - 1)) & ~(uintptr_t)(effective_alignment - 1);
    header = (allocation_header_t *)(aligned_address - metadata_size);
    header->base = base;
    header->size = size;

    return (void *)aligned_address;
}

void *allocator_reallocate(allocator_t *allocator, void *original, size_t size, size_t alignment)
{
    void *reallocated = NULL;
    allocation_header_t *header = NULL;
    size_t bytes_to_copy = 0;

    if (original == NULL)
    {
        return allocator_allocate(allocator, size, alignment);
    }

    if (size == 0)
    {
        allocator_deallocate(allocator, original);
        return NULL;
    }

    header = get_allocation_header(original);
    reallocated = allocator_allocate(allocator, size, alignment);
    if (reallocated == NULL)
    {
        return NULL;
    }

    bytes_to_copy = header->size < size ? header->size : size;
    memcpy(reallocated, original, bytes_to_copy);
    allocator_deallocate(allocator, original);
    return reallocated;
}

void allocator_deallocate(allocator_t *allocator, void *memory)
{
    allocation_header_t *header = NULL;

    if (memory == NULL)
    {
        return;
    }

    header = get_allocation_header(memory);
    free(header->base);
}

void allocator_notify_allocation(allocator_t *allocator, size_t size)
{
    logger_log(allocator->logger, LOGGER_SEVERITY_DEBUG, "Allocated %zu bytes", size);
}

void allocator_notify_deallocation(allocator_t *allocator, size_t size)
{
    logger_log(allocator->logger, LOGGER_SEVERITY_DEBUG, "Deallocated %zu bytes", size);
}
