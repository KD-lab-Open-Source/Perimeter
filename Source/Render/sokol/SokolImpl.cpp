// This file imports sokol header with implementation def set
#include <string>
#include "xerrhand.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT xassert
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#include <sokol_gfx.h>
#include <sokol_log.h>
#pragma GCC diagnostic pop
