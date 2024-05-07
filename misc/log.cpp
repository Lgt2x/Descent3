/*
* Descent 3 
* Copyright (C) 2024 Descent Developers
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "log.h"

#include "spdlog/sinks/stdout_sinks.h" // IWYU pragma: keep
#include "spdlog/spdlog.h"             // IWYU pragma: keep

#include <memory>
#include <string>

void InitLog() {
    // Set the logging pattern
    spdlog::set_pattern("%T [%l] %s:%!:%# %v");

    // Create and register a console logger
    auto console = spdlog::stdout_logger_mt("console");
    console->info("Logger initialized");
}