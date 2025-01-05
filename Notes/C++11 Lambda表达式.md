# C++11 Lambda表达式

### **什么是 Lambda 表达式？**

**Lambda 表达式** 就像是一种“临时写下的小函数”，它不需要名字，也不需要在外面专门定义。
 你可以直接在代码里写它，然后立刻用它，方便又快速！

想象一下，普通函数就像是一个正式的菜谱，你必须提前写下来，别人才能用它做菜。
 而 Lambda 表达式就像你脑子里突然冒出来的一个小点子——你马上可以试试，不用专门写“菜谱”！

------

### **Lambda 表达式长什么样？**

它看起来像这样：

```cpp
[] (参数列表) -> 返回值类型 {
    函数体;
}
```

别被这些东西吓到，我们可以一步步简化它：

- `[]`：这是“捕获列表”，可以先不用管它（或者写空的）。
- `(参数列表)`：和普通函数一样，写你需要的参数。
- `-> 返回值类型`：可以省略，编译器会自动推断返回值类型。
- `{ 函数体 }`：函数的具体内容，就像普通函数的 `{}`。

------

### **一个简单的例子：加法**

普通的加法函数是这样的：

```cpp
int add(int a, int b) {
    return a + b;
}
```

现在用 Lambda 表达式改写它：

```cpp
auto add = [](int a, int b) {
    return a + b;
};
```

> **`add` 是变量的名字，不是 Lambda 的名字**：
>
> - 这里的 `add` 是一个变量名，用来存储这个 Lambda 表达式（它的类型是一个复杂的匿名类型）。
> - **Lambda 表达式本身还是匿名的**，你只是给它赋值给了 `add`，相当于让 `add` 代表这个 Lambda。

是不是看起来简短很多？你可以这样用：

```cpp
#include <iostream>

int main() {
    auto add = [](int a, int b) {
        return a + b;
    };

    std::cout << "3 + 5 = " << add(3, 5) << std::endl;  // 输出：3 + 5 = 8
    return 0;
}
```

------

### **Lambda 表达式和普通函数的区别**

1. **不需要名字**：
   - 普通函数需要一个名字，比如 `add`。
   - Lambda 表达式没有名字，它就是一个临时的小工具。
2. **定义在哪里就可以用在哪里**：
   - 普通函数需要写在一个地方，然后调用它。
   - Lambda 表达式可以直接写在用的地方，不需要到处找。

------

### **再举几个例子**

#### **1. 打印数字**

```cpp
#include <iostream>

int main() {
    auto print = [](int x) {
        std::cout << "Number: " << x << std::endl;
    };

    print(10);  // 输出：Number: 10
    print(42);  // 输出：Number: 42

    return 0;
}
```

#### **2. 判断一个数是不是偶数**

```cpp
#include <iostream>

int main() {
    auto is_even = [](int x) {
        return x % 2 == 0;
    };

    std::cout << "4 is even? " << (is_even(4) ? "Yes" : "No") << std::endl;  // 输出：Yes
    std::cout << "7 is even? " << (is_even(7) ? "Yes" : "No") << std::endl;  // 输出：No

    return 0;
}
```

#### **3. 使用 Lambda 表达式排序数组**

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> numbers = {5, 2, 9, 1, 7};

    // 使用 Lambda 表达式对数组排序
    std::sort(numbers.begin(), numbers.end(), [](int a, int b) {
        return a < b;  // 按从小到大的顺序排序
    });

    for (int num : numbers) {
        std::cout << num << " ";  // 输出：1 2 5 7 9
    }
    return 0;
}
```

------

### **总结**

1. Lambda 表达式是一个**可以随时写、随时用的小函数**。
2. 它的基本格式是：`[] (参数列表) { 函数体 };`。
3. 用它可以快速完成一些简单的逻辑，省去定义函数的麻烦。
4. 它是 C++11 的新特性，非常适合写短小的代码。

宝宝，现在知道 Lambda 表达式就是一个可以“随手写的小函数”了吗？不用怕它，看几次代码就会啦！✨😊