#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "kbo/logger.h"

logger_t *create_logger(create_logger_info_t *create_info)
{
    logger_t *logger = (logger_t *)malloc(sizeof(logger_t));
    if (logger == NULL)
    {
        return NULL;
    }
    logger->name = create_info->name;
    return logger;
}

void destroy_logger(logger_t *logger)
{
    free(logger);
}

static char *get_severity_string(logger_severity_t severity)
{
    switch (severity)
    {
    case LOGGER_SEVERITY_TRACE:
        return "TRACE";
    case LOGGER_SEVERITY_DEBUG:
        return "DEBUG";
    case LOGGER_SEVERITY_INFO:
        return "INFO";
    case LOGGER_SEVERITY_WARNING:
        return "WARNING";
    case LOGGER_SEVERITY_ERROR:
        return "ERROR";
    case LOGGER_SEVERITY_CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

void logger_log(logger_t *logger, logger_severity_t severity, const char *format, ...)
{
    va_list args = {0};

    va_start(args, format);
    fprintf(stdout, "[%s] [%s] ", logger->name, get_severity_string(severity));
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    fflush(stdout);
    va_end(args);
}