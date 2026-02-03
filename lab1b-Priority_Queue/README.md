тут можно запустить программу с гугл-тестами, а можно с main.cpp, в которой тоже реализованы тесты

## сборка

```
rm -rf build
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

## запуск

```
./lab1b.exe - запуск с main.cpp
./tests.exe - запуск гугл-тестов
```

## список тестов:

- ТЕСТЫ НА ПУСТУЮ ОЧЕРЕДЬ
- MAX-HEAP: push / pop / top
- MIN-HEAP: push / pop / top
- КОНСТРУКТОР ОТ ВЕКТОРА
- КОПИРОВАНИЕ / ПЕРЕМЕЩЕНИЕ
- clear / reserve / empty / size
- is_max_heap()
- operator== / operator!=
- Исключения отдельно для top/pop на пустой
