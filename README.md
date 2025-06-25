# 🧠 Coursework: Knapsack Algorithms (3rd year)

Курсовой проект по реализации и сравнению различных алгоритмов решения задачи о рюкзаке (Knapsack Problem) на C++.

## 📌 Описание

Проект реализует 5 алгоритмов:
- `2-Approximation` — жадный
- `Dynamic Programming` — точный
- `FPTAS` — приближённый со сжатием
- `Branch and Bound` — перебор с отсечением
- `Genetic Algorithm` — эволюционный

Сравнение проводится по времени и точности на 8 тестах (p01–p08). Результаты сохраняются в `src/results.csv`.

## ⚙️ Технологии

- Язык: C++17
- Сборка: Makefile
- Проверка памяти: Valgrind
- Визуализация: Python (pandas, matplotlib)

## 🚀 Как запустить

### 1. Сборка проекта

```bash
make
```

Или вручную:

```bash
g++ -std=c++17 -O2 -o build/knapsack_solver src/main.cpp
```

### 2. Запуск основного скрипта

```bash
./build/knapsack_solver
```

Результаты сохраняются в `src/results.csv`.

## 🔍 Проверка на утечки памяти (Valgrind)

Скрипт `check_memory.sh` поддерживает 3 режима:

- `fast` — поверхностная проверка (по умолчанию)
- `full` — глубокая проверка всех утечек
- `debug` — полный анализ с трассировкой

### Запуск:

```bash
bash check_memory.sh fast
bash check_memory.sh full
bash check_memory.sh debug
```

Внутри скрипт автоматически:
- пересобирает проект с отладочными флагами
- запускает `valgrind --tool=memcheck` с нужными параметрами

## 📁 Структура

```
Coursework-3rd-year/
├── src/
│   ├── main.cpp          # Реализация всех алгоритмов
│   ├── data/             # Тестовые наборы p01–p08
│   └── results.csv       # Итоговый результат
├── check_memory.sh       # Анализ памяти
├── Makefile              # Сборка
├── README.md             # Документация
```

## 📎 Автор

**Степанов Денис**, 22ПМИ-1  
НИУ ВШЭ, Нижний Новгород  
Научный руководитель: Бацына Е. К.