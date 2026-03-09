#pragma once

typedef struct create_logger_info_s
{
    const char *name;
} create_logger_info_t;

typedef struct logger_s
{
    const char *name;
} logger_t;

typedef enum logger_severity_e
{
    LOGGER_SEVERITY_TRACE = 0,
    LOGGER_SEVERITY_DEBUG = 1,
    LOGGER_SEVERITY_INFO = 2,
    LOGGER_SEVERITY_WARNING = 3,
    LOGGER_SEVERITY_ERROR = 4,
    LOGGER_SEVERITY_CRITICAL = 5
} logger_severity_t;

logger_t *create_logger(create_logger_info_t *create_info);
void destroy_logger(logger_t *logger);

void logger_log(logger_t *logger, logger_severity_t severity, const char *format, ...);