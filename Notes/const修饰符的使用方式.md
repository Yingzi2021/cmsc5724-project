# const修饰符的使用方式

```c++
class Point {
public:
    double x, y;

    // 构造函数
    Point(double x, double y) : x(x), y(y) {}

    // 计算两个点之间的距离
    double distance(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
```

`const`修饰符

- `const Point& other`：表示 `other` 是一个对 `Point` 类型对象的 **常量引用**，也就是说，在这个函数中，你不能修改 `other` 的内容。
- `const` （第二个）： 表示 **`distance` 是一个常量成员函数**，它承诺在调用该函数时，不会修改**调用这个函数的对象**（即 `this` 指针所指向的对象）。
- 如果一个对象被声明为 `const` (例如,定义一个 `const Point p1(1, 2);` )，只有它的 **`const` 成员函数** 可以被调用。因为 `p1` 是一个常量对象（`const Point`），编译器会禁止任何可能修改 `p1` 的操作。

> 如果你确定某个函数只需要读取对象的状态，而不需要修改它，就应该加上 `const`。这样可以防止误修改对象，增强代码的 **安全性** 和 **可读性**。