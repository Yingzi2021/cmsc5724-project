# `static` 的使用总结

| **场景**         | **含义**                                                     |
| ---------------- | ------------------------------------------------------------ |
| **静态成员变量** | 类级别的变量，所有对象共享，生命周期贯穿程序运行。           |
| **静态成员函数** | 类级别的函数，不依赖具体对象，只能访问静态成员变量和其他静态函数。 |
| **静态局部变量** | 函数内部的静态变量，生命周期贯穿程序运行，但作用域仍然是函数内。 |
| **静态全局变量** | 文件级别的变量，只能在当前文件中访问，防止全局变量冲突。     |
| **静态全局函数** | 文件级别的函数，只能在当前文件中调用，隐藏实现细节，防止命名冲突。 |

## 静态成员变量

**概念：**

- 静态成员变量是**类级别**的，而不是对象级别的。
- 特点：
  1. **所有对象共享一个静态成员变量，只有一个实例**。
  2. 它在程序运行期间一直存在，不随对象的创建和销毁而变化。
  3. 必须在**类外**定义和初始化。

**例子：统计 `Point` 类创建了多少个点**

```c++
#include <iostream>

class Point {
public:
    double x, y;

    // 静态成员变量
    static int count;

    Point(double x, double y) : x(x), y(y) {
        count++;  // 每创建一个对象，count++
    }

    ~Point() {
        count--;  // 每销毁一个对象，count--
    }

    // 静态成员函数：获取当前计数
    static int getCount() {
        return count;
    }
};

// 静态成员变量在类外初始化
int Point::count = 0;

int main() {
    Point p1(1, 2), p2(3, 4);
    std::cout << "Current count: " << Point::getCount() << std::endl;

    {
        Point p3(5, 6);
        std::cout << "Current count: " << Point::getCount() << std::endl;
    } // p3 超出作用域被销毁

    std::cout << "Current count: " << Point::getCount() << std::endl;
    return 0;
}
```

**输出：**

```
Current count: 2
Current count: 3
Current count: 2
```

**关键点：**

- `static int count` 是所有 `Point` 对象共享的，不属于单个对象。
- 静态变量初始化必须在类外进行，写成 `int Point::count = 0;`。

## 静态成员函数

**概念：**

- 静态成员函数是类级别的函数，不依赖于任何对象。
- 特点：
  1. 只能访问静态成员变量或调用其他静态函数（因为它和对象无关）。
  2. 可以通过类名直接调用，而不需要实例化对象。

**例子：计算两点之间的距离**

```c++
#include <iostream>
#include <cmath>

class Point {
public:
    double x, y;

    Point(double x, double y) : x(x), y(y) {}

    // 静态成员函数：计算两点之间的距离
    static double distance(const Point& p1, const Point& p2) {
        double dx = p1.x - p2.x;
        double dy = p1.y - p2.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

int main() {
    Point p1(1, 2), p2(4, 6);
    std::cout << "Distance: " << Point::distance(p1, p2) << std::endl;
	//注意它的调用方式
    return 0;
}
```

**输出：**

```
Distance: 5
```

**关键点：**

- `distance` 是静态的，因为它不需要依赖任何 `Point` 对象。
- 通过 `Point::distance(p1, p2)` 调用，无需创建 `Point` 对象。

## 静态局部变量

**概念：**

- 静态局部变量的生命周期是整个程序运行期间（不像普通局部变量在函数退出时销毁）。
- 在函数第一次调用时初始化，后续调用会保留之前的值。

**例子：统计某个函数被调用的次数**

```c++
#include <iostream>

class Point {
public:
    double x, y;

    Point(double x, double y) : x(x), y(y) {}

    void print() const {//这里const修饰类成员函数print
        static int callCount = 0;  // 静态局部变量
        callCount++;
        std::cout << "Point(" << x << ", " << y << ") - called " << callCount << " times" << std::endl;
    }
};

int main() {
    Point p1(1, 2), p2(3, 4);

    p1.print();
    p2.print();
    p1.print();

    return 0;
}
```

**输出：**

```
Point(1, 2) - called 1 times
Point(3, 4) - called 2 times
Point(1, 2) - called 3 times
```

**关键点：**

- `callCount` 是静态局部变量，函数多次调用时不会被重新初始化。
- 它在程序运行期间只初始化一次，后续调用会记住之前的值。

## 静态全局变量（文件作用域静态变量）

**概念：**

- 如果在全局作用域用 `static` 修饰一个变量或函数，那么它的 **作用域只限于当前文件**。(重要！！！)
- 这与普通全局变量不同，普通全局变量会在整个程序中可见。

**例子：静态全局变量**

```c++
#include <iostream>

static int globalStaticVar = 42;  // 静态全局变量，作用域仅限本文件

void printGlobal() {
    std::cout << "Global Static Var: " << globalStaticVar << std::endl;
}

int main() {
    printGlobal();
    return 0;
}
```

如果有另一个文件尝试访问 `globalStaticVar`，编译会报错，因为它的作用域被限制在定义它的文件中。

## 静态函数（文件作用域静态函数）

**概念：**

- 和静态全局变量类似，`static` 修饰的函数只能在声明它的文件中使用。
- **用于隐藏实现细节，避免名称冲突。（重要！！！）**

**例子：静态函数**

```c++
#include <iostream>

static void helperFunction() {  // 静态函数，只能在本文件中使用
    std::cout << "This is a static function." << std::endl;
}

int main() {
    helperFunction();  // 可以正常调用
    return 0;
}
```

如果另一个文件试图调用 `helperFunction`，会报错，因为它的作用域只限于当前文件。

**静态和链接（区别全局函数）**

- 当 `static` 修饰全局变量或函数时，它具有 **内部链接**（internal linkage），这意味着它在编译时会被限制在当前文件中，而不暴露给其他编译单元。
- 普通全局变量和函数具有 **外部链接**（external linkage），可以在多个文件间共享。

> Intuition：
>
> 在一个神奇的玩具工厂里，工人们每天忙碌地制造着各式各样的机器人。每个机器人都有自己的电量指示灯，用来显示它当前的能量状态。有的机器人电量满满的，还能跳舞、唱歌；有的机器人电量快耗尽了，需要赶紧去充电。然而，整个工厂里只有一个充电站，所有机器人共享它的功率。这就像静态成员变量，虽然每个机器人都有自己的属性，但某些资源（比如充电站的功率）是整个工厂级别的，所有机器人都依赖于它，它并不属于单个机器人。
>
> 工厂里还有一个负责统计的系统，用来记录已经生产了多少个机器人。这个系统跟机器人本身没什么关系，它只是关心“工厂”生产的整体情况。这就像静态成员函数，它不依赖具体的机器人实例，而是专注于处理工厂级别的工作，比如统计生产数量、分析生产效率等。
>
> 在维修间里，工厂还有一本特别的记账本，用来记录维修了多少次。这个记账本非常神奇，虽然它看起来是维修间的物品（就像一个局部变量），但它的记录是永久保存的，每次维修都会在之前的记录上继续增量。也就是说，无论维修间开了多少次门，这本记账本上的数据都不会被清空。这正是静态局部变量的特点：它属于一个函数，但它的状态会跨函数调用而保留。
>
> 此外，工厂里还有一个深藏的秘密：制造机器人的核心配方。这个配方是工厂独有的技术秘密，只有工厂内部的工作人员才能使用，外界的人完全无法窥探。就像静态全局变量，它虽然属于工厂的一部分，但其作用域被严格限制在工厂内部，其他地方的人根本无法访问它。
>
> 总的来说，`static` 这个关键字的精髓在于它能够区分“属于某个对象”还是“属于整个系统”。有些资源是所有对象共享的，有些工具是系统级别的操作，还有些状态是局部的却能持久保存。理解这些角色和分工后，`static` 就不再神秘，而是一个逻辑清晰的好帮手。