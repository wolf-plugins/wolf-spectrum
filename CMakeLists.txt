# SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.7)

project(wolf-spectrum)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(dpf)
add_subdirectory(src)
