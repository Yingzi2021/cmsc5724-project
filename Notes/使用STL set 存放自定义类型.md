# 使用STL set 存放自定义类型

如果要在 `std::set` 中存储自定义结构体，必须为结构体定义一个 **排序规则**。因为 `std::set` 是一个有序集合，其内部使用红黑树进行元素的组织和管理，插入元素时需要根据排序规则来判断元素的相对顺序。

### **两种方式定义排序规则：**

#### **1. 重载 `<` 运算符**

- 为自定义结构体提供 `operator<` 重载，让 `std::set` 能够比较两个对象的大小。

```cpp
#include <iostream>
#include <set>
#include <string>

struct Person {
    std::string name;
    int age;

    // 重载 < 运算符，用于排序
    bool operator<(const Person& other) const {
        // 按年龄排序，若年龄相同则按名字排序
        return (age < other.age) || (age == other.age && name < other.name);
    }
};

int main() {
    std::set<Person> people;
    people.insert({"Alice", 30});
    people.insert({"Bob", 25});
    people.insert({"Alice", 25});  // 按 name 和 age 的排序规则存储

    for (const auto& person : people) {
        std::cout << person.name << " (" << person.age << ")\n";
    }
    return 0;
}
```

#### **2. 提供自定义比较函数**

- 如果你不想或者无法直接修改结构体，可以通过 `std::set` 的模板参数传入一个自定义的比较函数。

```cpp
#include <iostream>
#include <set>
#include <string>

struct Person {
    std::string name;
    int age;
};

// 自定义比较函数
struct ComparePerson {
    bool operator()(const Person& a, const Person& b) const {
        // 按年龄排序，若年龄相同则按名字排序
        return (a.age < b.age) || (a.age == b.age && a.name < b.name);
    }
};

int main() {
    std::set<Person, ComparePerson> people;
    people.insert({"Alice", 30});
    people.insert({"Bob", 25});
    people.insert({"Alice", 25});

    for (const auto& person : people) {
        std::cout << person.name << " (" << person.age << ")\n";
    }
    return 0;
}
```

------

### **注意事项**

1. 排序规则需要满足严格弱序性 (Strict Weak Ordering)：
   - 如果 `a < b` 为真，则 `b < a` 必须为假。
   - 如果 `a < b` 和 `b < c` 为真，则 `a < c` 也必须为真（传递性）。
2. 保持一致性：
   - 如果选择了重载 `<` 或自定义比较函数，确保与逻辑需求一致。例如，在插入和查找操作时，`std::set` 会完全依赖这个规则进行判断。
3. 避免歧义：
   - 如果你的结构体同时重载了 `<` 和提供了自定义比较函数，`std::set` 会优先使用自定义比较函数。

用这两种方式之一定义好排序规则后，就可以安全地将自定义结构体插入 `std::set` 中啦！😊