
# Prefetching

**Prefetching** — это проект в рамках КР, реализующий алгоритм предвыборки данных (Prefetcher). Проект разработан на языке C++ и включает в себя инструменты для генерации и анализа трасс памяти.

## 📁 Структура проекта

- `src/` — основной исходный код.
- `traces/` — каталог для хранения трасс памяти.
- `getTraces.sh` — скрипт для загрузки или генерации трасс.
- `Makefile` — файл для сборки проекта.

## ⚙️ Сборка и запуск

1. Убедитесь, что у вас Linux OS, установлен компилятор C++ (например, `g++`), утилита `make` и `valgrind`.

2. Склонируйте репозиторий:

   ```bash
   git clone https://github.com/funtik22/Prefetching.git
   cd Prefetching
   ```

3. Соберите проект:

   ```bash
   make
   ```

4. Сгенерируйте трассы памяти:

   ```bash
   chmod u+x ./getTraces.sh
   ./getTraces.sh
   ```

5. Запустите предвыборщик с использованием одной из трасс:

   ```bash
   ./Prefetcher --mode=[stride|corr|both] --cache=N tracefile
   ```
   * mode - алгоритм префетчинга
       * stride - Stride Prefetcher
       * corr - Markov Prefetcher
   * both - два префетчера одновременно
   * N - размер кэша
   * tracefile - файл с трассой памяти


## 📊 Пример использования

```
./Prefetcher --mode=corr --cache=10 ./traces/linkedList_trace.log
```

Пример вывода:

```
Markov Prefetcher:
  Loads: 16041
  Misses: 4316
  Prefetches: 10064
  Prefetch hits: 8488
  Useless prefetches: 1576
  Accuracy: 84.34%
  Coverage: 66.29%
```

## 📝 Лицензия

Этот проект распространяется под лицензией MIT. См. файл [LICENSE](LICENSE) для получения дополнительной информации.
