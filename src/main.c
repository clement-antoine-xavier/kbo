#include <stdlib.h>

#include <vulkan/vulkan.h>

#include "kbo/allocator.h"
#include "kbo/logger.h"

static void *allocation_function(void *user_data, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    return allocator_allocate((allocator_t *)user_data, size, alignment);
}

static void *reallocation_function(void *user_data, void *original, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    return allocator_reallocate((allocator_t *)user_data, original, size, alignment);
}

static void deallocation_function(void *user_data, void *memory)
{
    allocator_deallocate((allocator_t *)user_data, memory);
}

static void allocation_notification_function(void *user_data, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
    allocator_notify_allocation((allocator_t *)user_data, size);
}

static void deallocation_notification_function(void *user_data, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
    allocator_notify_deallocation((allocator_t *)user_data, size);
}

int main(int argc, char *argv[])
{
    logger_t *logger = NULL;
    allocator_t *allocator = NULL;
    VkResult result = VK_SUCCESS;
    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "KBO",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "KBO Engine",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };
    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
    };
    VkAllocationCallbacks instance_allocation_callbacks = {
        .pfnAllocation = allocation_function,
        .pfnReallocation = reallocation_function,
        .pfnFree = deallocation_function,
        .pfnInternalAllocation = allocation_notification_function,
        .pfnInternalFree = deallocation_notification_function,
    };
    VkInstance instance = {0};
    uint32_t physical_device_count = 0;
    VkPhysicalDevice *physical_devices = NULL;

    logger = create_logger(&(create_logger_info_t){
        .name = "main",
    });
    allocator = create_allocator(&(create_allocator_t){});
    instance_allocation_callbacks.pUserData = allocator;
    logger_log(logger, LOGGER_SEVERITY_INFO, "Creating Vulkan instance");
    result = vkCreateInstance(&instance_create_info, &instance_allocation_callbacks, &instance);
    if (result != VK_SUCCESS)
    {
        logger_log(logger, LOGGER_SEVERITY_ERROR, "Failed to create Vulkan instance: %d", result);
        return EXIT_FAILURE;
    }
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
    if (result != VK_SUCCESS)
    {
        logger_log(logger, LOGGER_SEVERITY_ERROR, "Failed to get physical device count: %d", result);
        return EXIT_FAILURE;
    }
    physical_devices = (VkPhysicalDevice *)allocator_allocate(allocator, sizeof(VkPhysicalDevice) * physical_device_count, alignof(VkPhysicalDevice));
    if (physical_devices == NULL)
    {
        logger_log(logger, LOGGER_SEVERITY_ERROR, "Failed to allocate memory for physical devices");
        return EXIT_FAILURE;
    }
    result = vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);
    if (result != VK_SUCCESS)
    {
        logger_log(logger, LOGGER_SEVERITY_ERROR, "Failed to enumerate physical devices: %d", result);
        return EXIT_FAILURE;
    }
    for (uint32_t i = 0; i < physical_device_count; i++)
    {
        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);
        logger_log(logger, LOGGER_SEVERITY_INFO, "Found physical device: %s", properties.deviceName);
        logger_log(logger, LOGGER_SEVERITY_DEBUG, "Device type: %d", properties.deviceType);
        logger_log(logger, LOGGER_SEVERITY_DEBUG, "API version: %d.%d.%d", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
    }
    if (result == VK_SUCCESS)
    {
        vkDestroyInstance(instance, &instance_allocation_callbacks);
    }
    allocator_deallocate(allocator, physical_devices);
    destroy_allocator(allocator);
    destroy_logger(logger);
    return EXIT_SUCCESS;
}