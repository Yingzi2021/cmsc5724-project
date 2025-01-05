# 使用unordered_set储存自定义类型

在 C++ 中使用 `std::unordered_set` 存放自定义类型（例如 `struct point`），需要解决以下两个问题：

1. **如何为自定义类型计算哈希值**。
2. **如何判断两个自定义类型是否相等**。

这是因为 `std::unordered_set` 是基于哈希表实现的，它依赖于以下两点来存储和查询数据：

- 哈希函数 (`std::hash`)：计算每个对象的哈希值，将其分配到相应的桶中。
- 相等比较函数 (`std::equal_to`)：判断两个对象是否相等，以处理哈希冲突。

------

## 注意点 1：定义相等性比较

`std::unordered_set` 通过 `operator==` 或自定义的 `std::equal_to` 来判断两个对象是否相等。如果两个对象被认为相等，则它们不能同时存在于集合中。

**示例**

```cpp
struct point {
    int x, y;

    // 自定义相等性比较
    bool operator==(const point& other) const {
        return x == other.x && y == other.y;
    }
};
```



## 注意点 2：自定义哈希函数

默认情况下，`std::hash` 只支持内置类型（如 `int`, `double`, `std::string`）。对于自定义类型，你需要为 `std::hash` 进行**模板特化**，以提供计算哈希值的逻辑。

**示例**

```cpp
#include <unordered_set>
#include <iostream>

struct point {
    int x, y;

    // 重载 operator==，定义相等性
    bool operator==(const point& other) const {
        return x == other.x && y == other.y;
    }
};

// 在 std 命名空间中 **特化** std::hash
namespace std {
    template <>
    struct hash<point> {
        size_t operator()(const point& p) const {
            // 自定义哈希函数
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1); // 组合哈希
        }
    };
}
```

细节：

1. **组合哈希**：

   - `hash<int>()(p.x)` 计算 `x` 的哈希值。

   - `hash<int>()(p.y)` 计算 `y` 的哈希值。

   - 使用异或操作符 `^` 和移位操作符 `<<` 组合 `x` 和 `y` 的哈希值，减少冲突的可能性。

   - **为了避免冲突，可以使用更复杂的hash函数例如：**

     ```c++
     size_t operator()(const point& p) const {
         return std::hash<int>()(p.x) * 31 + std::hash<int>()(p.y); // 31 是常用的哈希因子
     }
     ```

     > `size_t operator()(const point& p) const`这个函数重载了`()`即函数调用操作符，用于定义类对象的行为，使它们像函数一样可以被调用。
     >
     > ### **什么是 `operator()`?**
     >
     > `operator()` 是函数调用运算符，它允许你像调用函数一样调用一个对象。比如：
     >
     > ```c++
     > struct Functor {
     >     void operator()(int x) const {
     >         std::cout << "Called with " << x << std::endl;
     >     }
     > };
     > 
     > int main() {
     >     Functor functor;
     >     functor(42);  // 像函数一样调用对象
     >     return 0;
     > }
     > ```
     >
     > 输出：`Called with 42`
     >
     > 对于 `std::unordered_set`，哈希函数是一个**仿函数**（也叫**函数对象**），必须能够像函数一样接受一个参数，并返回哈希值（`size_t` 类型）。
     >
     > ### `functor(42)`  VS `std::hash<int>()(p.x)`区别：
     >
     > | **调用方式**            | **操作流程**                                                 |
     > | ----------------------- | ------------------------------------------------------------ |
     > | `functor(42);`          | 已有对象 `functor`，直接调用它的 `operator()`，无需额外的对象创建。 |
     > | `std::hash<int>()(42);` | 需要先通过 `std::hash<int>()` 创建一个临时对象，然后调用它的 `operator()`。 |

2. **标准库限制**：

   - 必须在 `std` 命名空间中**特化** `std::hash`，否则标准库无法识别。

**模板类特化（Template Specialization）**是指为一个模板类或模板函数提供**特定类型的特殊实现**。通常，模板类是为一组通用类型设计的，但有时某些类型需要特殊处理，模板特化就是为了解决这个问题。想象一个工具箱，模板是一个多功能工具，适用于很多情况；而模板特化是专门定制的工具，只适用于某些特定的情况。	

**为了让 `std::unordered_set` 支持自定义类型 `point`，需要为标准库中的模板类 `std::hash` 进行特化。**

（1）**什么是 `std::hash`？**

`std::hash` 是一个**模板类**，标准库为许多常用类型（如 `int`、`std::string` 等）提供了默认实现，用来计算哈希值。

> 模板类是一种**泛型**的类，它是为多种类型设计的，可以在实例化时指定具体的类型。它的代码会在**编译时**根据具体的类型生成相应的类。同时，`std::hash` 是一个工具类，这意味着：
>
> - `std::hash` 的职责是提供一个函数接口（`operator()`），用于计算哈希值。
> - 它没有数据成员，因为它不需要存储状态，只需要一个行为（计算哈希）。

默认情况下：

```cpp
std::hash<int>()(42);  // 返回一个整型的哈希值(对()运算符的重载，int operator()(int x) const;)
std::hash<std::string>()("hello");  // 返回一个字符串的哈希值 (还是对()运算符的重载，int operator()(int x) const;)
```

但对于自定义类型（如 `struct point`），标准库没有内置支持，这时需要**模板类特化**。

（2）**示例代码**

1. 定义一个自定义类型 `point`

```cpp
struct point {
    int x, y;

    // 重载相等运算符，用于判断两个点是否相等
    bool operator==(const point& other) const {
        return x == other.x && y == other.y;
    }
};
```

2. 特化 `std::hash` 模板类

```cpp
namespace std {
    template <>
    struct hash<point> {
        size_t operator()(const point& p) const {
            // 自定义哈希函数：组合 x 和 y 的哈希值
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
}
```

- `template <>` 表示我们正在特化 `std::hash` 模板类。
- operator()是哈希函数，用来计算 point的哈希值：
  - `std::hash<int>()(p.x)`：计算 `x` 的哈希值。
  - `std::hash<int>()(p.y)`：计算 `y` 的哈希值。
  - 使用异或 (`^`) 和移位操作 (`<<`) 来组合这两个哈希值。