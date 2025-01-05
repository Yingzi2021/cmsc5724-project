# `const` 的常见用法

------

| **用法**         | **形式**                     | **含义**                                       |
| ---------------- | ---------------------------- | ---------------------------------------------- |
| 常量变量         | `const int a = 10;`          | `a` 是只读变量，不能修改。                     |
| 常量指针指向的值 | `const int* p = &x;`         | 指针指向的值是只读的。                         |
| 常量指针         | `int* const p = &x;`         | 指针本身不能改变指向，但指针指向的值可以修改。 |
| 常量引用参数     | `void func(const int& ref);` | 函数参数只读，按引用传递避免拷贝。             |
| 返回值是常量     | `const int func();`          | 函数返回值不能作为左值修改。                   |
| 常量成员函数     | `void func() const;`         | 表示函数不会修改类的成员变量。                 |

### **1. const 修饰变量**

**形式：**

```cpp
const 类型 变量名 = 值;
```

**作用：**

表示变量的值是**只读的**，不能被修改。

**例子：**

```cpp
const int a = 10;  // a 是常量，不能修改
a = 20;            // ❌ 错误，编译器会报错
```

### **2. const 修饰指针**

指针的 `const` 修饰有三种常见的形式：

#### **（1）指针指向的值是 const**

**形式：**

```cpp
const 类型* 指针名;
```

指针指向的**值**不能修改，但指针本身可以指向别的地址。

**例子：**

```cpp
int x = 10, y = 20;
const int* p = &x;  // 指针 p 指向的值是 const，不可修改
*p = 15;            // ❌ 错误
p = &y;             // ✅ 指针本身可以改变
```

------

#### **（2）指针本身是 const**

**形式：**

```cpp
类型* const 指针名;
```

指针本身不可改变，但指向的值可以修改。

**例子：**

```cpp
int x = 10, y = 20;
int* const p = &x;  // 指针 p 是 const，不能改变指向
p = &y;             // ❌ 错误，指针不能改变
*p = 15;            // ✅ 指向的值可以修改
```

------

#### **（3）指针和指向的值都是 const**

**形式：**

```cpp
const 类型* const 指针名;
```

指针本身和指向的值都不能修改。

**例子：**

```cpp
int x = 10;
const int* const p = &x;  // p 是完全 const
p = &x;                   // ❌ 错误，指针不能改变
*p = 15;                  // ❌ 错误，值也不能修改
```

------

### **3. const 修饰函数参数**

#### **（1）按值传递的参数**

**形式：**

```cpp
void func(const int value);
```

**作用：** 表示传入的值在函数中是只读的，不能被修改。

**例子：**

```cpp
void printValue(const int value) {
    // value 不能修改
    // value = 10;  // ❌ 错误
    std::cout << value << std::endl;
}
```

------

#### **（2）按引用传递的参数**

**形式：**

```cpp
void func(const int& ref);
```

**作用：** 按引用传递，避免拷贝，提高效率，同时保证参数不被修改。

**例子：**

```cpp
void printValue(const int& value) {
    // value 是只读的
    // value = 10;  // ❌ 错误
    std::cout << value << std::endl;
}
```

#### **（3）指针参数**

指针参数可以结合上面提到的三种指针修饰方式：

- `const int* p`：指针指向的值是只读的。
- `int* const p`：指针本身是只读的。
- `const int* const p`：指针本身和指向的值都是只读的。

**例子：**

```cpp
void func(const int* p);       // 只读指针
void func(int* const p);       // 只读指针变量
void func(const int* const p); // 完全只读
```

------

### **4. const 修饰函数返回值**

#### **（1）返回值是 const**

**形式：**

```cpp
const int func();
```

**作用：** 函数**返回的值是只读**的，不能作为左值修改。

**例子：**

```cpp
const int getValue() {
    return 10;
}

int main() {
    const int value = getValue();  // ✅ OK
    // getValue() = 20;  // ❌ 错误，返回值不能修改
}
```

------

### **5. const 修饰类成员函数**

**形式：**

```cpp
class MyClass {
    void func() const;
};
```

**作用：** 表示成员函数不会修改类的成员变量。

**例子：**

```cpp
class Point {
    int x, y;
public:
    Point(int x, int y) : x(x), y(y) {}

    void print() const {       // 常量成员函数
        std::cout << x << ", " << y << std::endl;
        // x = 10;  // ❌ 错误，不能修改成员变量
    }
};
```

**常量对象只能调用常量成员函数**

```cpp
const Point p(1, 2);
p.print();  // ✅ OK
```

------

### **6. const 在前和在后的区别**

其实 `const` 在前和在后的含义没有本质区别，C++ 允许灵活的写法。

**例子：**

```cpp
const int a = 10;  // 常用写法
int const b = 20;  // 等价写法
```

**注意：**

对于指针修饰，顺序非常重要：

- `const int* p` 和 `int* const p` 是完全不同的意思！
- 阅读时从右往左看会更直观：
   `const int* p` = “指向常量的指针”；
   `int* const p` = “常量指针”。