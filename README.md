# Simple Vector
Реализация собственного контейнера - упрощенного аналога std::vector с использованием RAII-обертки над массивом в динамической памяти.

### Реализованный функционал
- Конструктор по умполчанию.
- Параметризированный конструктор.
- Конструктор из std::initializer_list.
- Конструктор копирования.
- Оператор присваивания.
- Резервирующий конструктор.
- Move-семантика.
- Метод PushBack - добавляет элемента в конец массива.
- Метод PopBack - удаляет последний элемент массива.
- Метод Insert - вставляет элемент в произвольной позиции ветора
- Метод Erase - удаляет элемент в произвольной позиции.
- Метод swap - обмен содержимого двух списков.
- Метод GetSize - возвращает количество элементов в векторе.
- Метод GetCapacity - для получения вместимости вектора.
- Метод Reserve - задаёт ёмкость ветктора
- Метод IsEmpty - возвращает true, если вектор пустой.
- Метод At - для доступа к элементу вектора по индексу. Имеет две версии: константную и неконстантную. В случае выхода за пределы массива, выбасывает исключение std::out_of_range.
- Метод Clear - очистка содержимого вектора без изменения его вместимости.
- Метод Resize - для изменения количества элементов в векторе. Предоставляет полную гарантию безопасности исключений.
- Метод begin, end, cbegin, cend - возвращают итераторы на начало и конец массива
- Деструктор. При разрушении вектора освобождается память, занимаемая его элементами.
- Операторы сравнения ==, !=, <, >, <=, >=.

### Использованные технологии
- C++ 17
- STL
- CMake 3.25.2

### Содержание файлов
- `simple_vector.h` шаблонный класс SimpleVector, ReserveProxyObj дополнительный класс-обёртка, чтобы компилятор мог вызвать правильный конструктор и вспомогательная функция Reserve.
- `array_ptr.h` класс ArrayPtr для создания и управления динамическим массивом, который лежит в основе SimpleVector.
- `main.cpp` точка входа и вспомогательные функции и классы для тестирования корректности работы методов.
- `CMakeLists.txt` файл с информацией для сборки проекта с помощью CMake

### Сборка проекта
Проект можно удобно собрать с помощью CMake версии 3.24 и выше, используя файл CMakeLists.txt

### Руководство по использованию
1. Для начала работы со списком необходимо создать объект класса SimpleVector, используя один из следующих конструкторов:

Конструктор по умолчанию
```C++
SimpleVector() noexcept = default;
```
Пример использования:
```C++
SimpleVector<int> v;
```
Конструктор создания вектора указанного размера
```C++
explicit SimpleVector(size_t size);
```
Пример использования:
```C++
SimpleVector<int> v(5);
```
Резервирующий конструктор
```C++
SimpleVector(ReserveProxyObj capacity);
```
Пример использования:
```C++
SimpleVector<int> v(Reserve(5)); // size = 0, capacity = 5
```
Конструктор, создающий вектор из size элементов, инициализированных value
```C++
SimpleVector(size_t size, const Type& value); //Type - шаблонный параметр
```
Пример использования:
```C++
SimpleVector<int> v(3, 42);
```
Конструктор из списка std::initializer_list
```C++
SimpleVector(std::initializer_list<Type> init); //Type - шаблонный параметр
```
Пример использования:
```C++
SimpleVector<int> v{1, 2, 3};
```
Копирующий конструктор
```C++
SimpleVector(const SimpleVector& other);
```
Пример использования
```C++
SimpleVector<int> numbers{1, 2};
SimpleVector numbers_copy(numbers);
```
Перемещающий конструктор
```C++
SimpleVector(SimpleVector&& rhs);
```
Пример использования
```C++
SimpleVector<X> vector_to_move; // X - noncopiable object
... 
SimpleVector<X> moved_vector = std::move(vector_to_move);
```

2. Методы вектора вызываются через точку, подобно методам контейнеров библиотеки STL.

Пример использования:
```C++
SimpleVector<int> v {8, 11, 2, 7};
v.Clear(); 
```
После вызова этого метода Clear вектор будет пуст, иметь нулевой размер, но значение capacity останется без изменений.

### Планы по доработке
- Сделать из Simple Smart
- Рефакторинг кода
