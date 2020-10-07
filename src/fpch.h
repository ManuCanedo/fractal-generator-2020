#pragma once

#include <windows.h>
#include <memory>
#include <future>
#include <functional>
#include <iostream>
#include <fstream>
#include <utility>

#include <algorithm>
#include <math.h>
#include <cstdint>
#include <complex>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

// Logging
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
#include "tools/Log.h"

// Metrics
#include "tools/Timer.h"

// Rendering
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Events
#include "events/Event.h"
#include "events/WindowEvent.h"
#include "events/KeyEvent.h"
#include "events/MouseEvent.h"




