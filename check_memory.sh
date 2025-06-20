#!/bin/bash

MODE=$1

VALGRIND_FLAGS="--leak-check=summary"
CXXFLAGS="-std=c++17 -O0 -g"

if [ "$MODE" = "fast" ]; then
    echo "🚀 Режим: FAST (поверхностная проверка)"
    VALGRIND_FLAGS="--leak-check=yes"
elif [ "$MODE" = "full" ]; then
    echo "🔍 Режим: FULL (глубокая проверка всех утечек)"
    VALGRIND_FLAGS="--leak-check=full --show-leak-kinds=all"
elif [ "$MODE" = "debug" ]; then
    echo "🧠 Режим: DEBUG (все утечки + трассировка)"
    VALGRIND_FLAGS="--leak-check=full --show-leak-kinds=all --track-origins=yes"
else
    echo "⚠️  Не указан режим или он неизвестен. Используется fast по умолчанию."
    VALGRIND_FLAGS="--leak-check=yes"
fi

echo "🔧 Компиляция с $CXXFLAGS..."
make clean
make CXXFLAGS="$CXXFLAGS"

echo "🚦 Запуск Valgrind с флагами:"
echo "    valgrind --tool=memcheck $VALGRIND_FLAGS ./build/knapsack_solver"
valgrind --tool=memcheck $VALGRIND_FLAGS ./build/knapsack_solver