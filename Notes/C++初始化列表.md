# C++初始化列表

```c++
class Point {
public:
    int x, y;
    Point() : x(0), y(0) {} //初始化列表（Initialization List）
};
```

## 初始化列表和赋值的区别

### （1）工作原理的区别

**初始化列表：**

- 在对象的构造阶段，直接初始化成员变量。
- 变量只被初始化一次。

**赋值：**

- 先调用默认构造函数创建成员变量（初始化为默认值）。
- 然后再通过赋值操作修改成员变量的值。

### （2）性能差异

对于简单类型（如 `int` 和 `double`），两种方式性能差异不大。但对于复杂类型（如自定义类或动态分配的资源），**初始化列表性能更优**，因为**避免了多次构造和赋值操作**。

举个例子：

```c++
class Data {
public:
    Data(int value) { /* 假设这里有复杂的构造逻辑 */ }
};

class Example {
private:
    Data dataMember;

public:
    // 使用初始化列表（推荐）
    Example() : dataMember(42) {}

    // 使用赋值（不推荐）
    Example(bool useAssignment) {
        dataMember = Data(42);  // 先默认构造，再赋值
    }
};
```

**问题：**

- 初始化列表直接用 `Data(42)` 初始化 `dataMember`。
- 赋值方式会先调用 `Data` 的默认构造函数，再调用赋值操作，性能较差。

### （3）某些场景必须使用初始化列表

有些成员变量**必须**使用初始化列表，比如：

- **常量成员变量**（`const`）；
- **引用成员变量**（`&`）；
- **没有默认构造函数的类成员**。

**例子：**

```c++
class Example {
public:
    const int value;  // 常量成员
    Example(int val) : value(val) {}  // 必须用初始化列表
};
```

如果尝试用赋值方式，会报错：

```
Example(int val) {
    value = val;  // ❌ 错误：const 变量不能赋值
}
```

## 初始化列表的其他优点

### （1）支持不同的构造逻辑

你可以在初始化列表中为不同的成员变量指定不同的初始值：

```c++
class Point {
public:
    int x, y;

    Point(int xVal, int yVal) : x(xVal), y(yVal) {}  // 初始化为给定值
    Point() : x(0), y(0) {}                          // 初始化为默认值
};
```

### （2）可与继承配合 (重要！)

在派生类的构造函数中，初始化列表还能用来调用基类的构造函数：

```c++
class Base {
public:
    Base(int value) {}
};

class Derived : public Base {
public:
    Derived(int value) : Base(value) {}  // 调用基类构造函数
};
```

### （3）支持成员变量的初始化顺序

初始化列表中的成员变量会**按照它们在类中的声明顺序**初始化，而不是在列表中的排列顺序。