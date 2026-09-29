#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cstddef>

template <typename T>
class Queue {
private:
    struct Node {
        T item;
        std::unique_ptr<Node> next;

        explicit Node(const T& value) : item(value), next(nullptr) {}
        explicit Node(T&& value) : item(std::move(value)), next(nullptr) {}
    };

    std::unique_ptr<Node> first_;
    Node* last_ = nullptr;   // observador no propietario
    std::size_t n_ = 0;

public:
    Queue() = default;

    // Copia
    Queue(const Queue& other) {
        for (const auto& item : other) {
            enqueue(item);
        }
    }

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            Queue tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Movimiento
    Queue(Queue&& other) noexcept
        : first_(std::move(other.first_)),
          last_(other.last_),
          n_(other.n_) {
        other.last_ = nullptr;
        other.n_ = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            first_ = std::move(other.first_);
            last_ = other.last_;
            n_ = other.n_;
            other.last_ = nullptr;
            other.n_ = 0;
        }
        return *this;
    }

    bool isEmpty() const noexcept {
        return first_ == nullptr;
    }

    std::size_t size() const noexcept {
        return n_;
    }

    T& peek() {
        if (isEmpty()) throw std::out_of_range("Queue underflow");
        return first_->item;
    }

    const T& peek() const {
        if (isEmpty()) throw std::out_of_range("Queue underflow");
        return first_->item;
    }

    void enqueue(const T& item) {
        auto node = std::make_unique<Node>(item);
        Node* newLast = node.get();

        if (last_ != nullptr) {
            last_->next = std::move(node);
        } else {
            first_ = std::move(node);
        }
        last_ = newLast;
        ++n_;
    }

    void enqueue(T&& item) {
        auto node = std::make_unique<Node>(std::move(item));
        Node* newLast = node.get();

        if (last_ != nullptr) {
            last_->next = std::move(node);
        } else {
            first_ = std::move(node);
        }
        last_ = newLast;
        ++n_;
    }

    T dequeue() {
        if (isEmpty()) throw std::out_of_range("Queue underflow");

        T item = std::move(first_->item);
        first_ = std::move(first_->next);
        --n_;

        if (first_ == nullptr) {
            last_ = nullptr;
        }
        return item;
    }

    void swap(Queue& other) noexcept {
        using std::swap;
        swap(first_, other.first_);
        swap(last_, other.last_);
        swap(n_, other.n_);
    }

    // Iterador
    template <bool IsConst>
    class Iterator {
        friend class Queue;
        template <bool> friend class Iterator;

        using NodePtr = std::conditional_t<IsConst, const Node*, Node*>;
        NodePtr current_ = nullptr;

        explicit Iterator(NodePtr current) : current_(current) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        Iterator() = default;

        template <bool OtherConst,
                  typename = std::enable_if_t<IsConst && !OtherConst>>
        Iterator(const Iterator<OtherConst>& other)
            : current_(other.current_) {}

        reference operator*() const {
            return current_->item;
        }

        pointer operator->() const {
            return &current_->item;
        }

        Iterator& operator++() {
            if (current_ != nullptr) {
                current_ = current_->next.get();
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.current_ == b.current_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    iterator begin() { return iterator(first_.get()); }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const { return const_iterator(first_.get()); }
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    std::string toString() const {
        std::ostringstream oss;
        bool firstItem = true;

        for (const auto& item : *this) {
            if (!firstItem) oss << ' ';
            oss << item;
            firstItem = false;
        }
        return oss.str();
    }
};

int main() {
    Queue<std::string> queue;
    std::string item;

    while (std::cin >> item) {
        if (item != "-") {
            queue.enqueue(std::move(item));
        } else if (!queue.isEmpty()) {
            std::cout << queue.dequeue() << ' ';
        }
    }

    std::cout << "(" << queue.size() << " left on queue)\n";
}