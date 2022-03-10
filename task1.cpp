#include <algorithm>
#include <cassert>
#include <cstddef>


using namespace std;

template<class T>
class Set {
private:
    static bool IsEqual(const T& a, const T&b) {
        return !(a < b) && !(b < a);
    }
    struct Node {
        T value;
        Node* left;
        Node* right;
        Node* parent;
        size_t size;
        int height;
        Node(const T& value): value(value), left(nullptr), right(nullptr), parent(nullptr),
                              size(1), height(1) {}
        Node(const T& value, Node* parent): value(value), left(nullptr), right(nullptr), parent(parent),
                                            size(1), height(1) {}
    };

    static Node* GetMax(Node* v) {
        if (v) {
            while (v->right) {
                v = v->right;
            }
        }
        return v;
    }

    static Node* GetMin(Node* v) {
        if (v) {
            while (v->left) {
                v = v->left;
            }
        }
        return v;
    }


    static size_t GetSize(Node* v) {
        if (v) {
            return v->size;
        }
        return 0;
    }
    static int GetHeight(Node* v) {
        if (v) {
            return v->height;
        }
        return 0;
    }

    static void Update(Node* v) {
        v->size = GetSize(v->left) + GetSize(v->right) + 1;
        v->height = max(GetHeight(v->left), GetHeight(v->right)) + 1;
    }

    static int Diff(Node* v) {
        if (!v) {
            return 0;
        }
        return GetHeight(v->left) - GetHeight(v->right);
    }

    static void ConLeft(Node* a, Node* b) {
        a->left = b;
        if (b) {
            b->parent = a;
        }
    }
    static void ConRight(Node* a, Node* b) {
        a->right = b;
        if (b) {
            b->parent = a;
        }
    }
    static void RotateLeft(Node* a) {
        Node* b = a->right;
        Node* par = a->parent;
        ConRight(a, b->left);
        ConLeft(b, a);
        b->parent = par;
        if (par) {
            if (par->left == a) {
                ConLeft(par, b);
            } else {
                ConRight(par, b);
            }
        }
        Update(a);
        Update(b);
    }
    static void RotateRight(Node* a) {
        Node* b = a->left;
        Node* par = a->parent;
        ConLeft(a, b->right);
        ConRight(b, a);
        b->parent = par;
        if (par) {
            if (par->left == a) {
                ConLeft(par, b);
            } else {
                ConRight(par, b);
            }
        }
        Update(a);
        Update(b);
    }
    static void BigRotateLeft(Node* a) {
        RotateRight(a->right);
        RotateLeft(a);
    }

    static void BigRotateRight(Node* a) {
        RotateLeft(a->left);
        RotateRight(a);
    }

    static Node* Rebalance(Node* a) {
        if (abs(Diff(a)) < 2) {
            return a;
        }
        if (Diff(a) == -2)  {
            Node* b = a->right;
            if (Diff(b) <= 0) {
                RotateLeft(a);
                return b;
            } else {
                Node* bl = b->left;
                BigRotateLeft(a);
                return bl;
            }
        } else if (Diff(a) == 2) {
            Node* b = a->left;
            if (Diff(b) >= 0) {
                RotateRight(a);
                return b;
            } else {
                Node* br = b->right;
                BigRotateRight(a);
                return br;
            }
        }
        throw;
    }

    Node* DfsCopy(Node* v) {
        if (!v) {
            return v;
        }
        Node* res = new Node(*v);
        res->left = DfsCopy(res->left);
        if (res->left) {
            res->left->parent = res;
        }
        res->right = DfsCopy(res->right);
        if (res->right) {
            res->right->parent = res;
        }
        return res;
    }

    void DfsDelete(Node* v) {
        if (!v) {
            return;
        }
        DfsDelete(v->left);
        DfsDelete(v->right);
        delete v;
        v = nullptr;
    }

    Node* InsertWalk(Node* v, const T& el) {
        if (!v) {
            return new Node(el);
        }
        if (IsEqual(v->value, el)) {
            return v;
        }
        if (v->value < el) {
            ConRight(v, InsertWalk(v->right, el));
            Update(v);
            return Rebalance(v);
        } else {
            ConLeft(v, InsertWalk(v->left, el));
            Update(v);
            return Rebalance(v);
        }
    }

    Node* Kill(Node* v) {
        Node* par = v->parent;
        if (par) {
            if (par->left == v) {
                par->left = nullptr;
            } else {
                par->right = nullptr;
            }
        }
        delete v;
        v = par;
        if (!v) {
            return nullptr;
        }
        while (true) {
            Update(v);
            v = Rebalance(v);
            if (v->parent) {
                v = v->parent;
            } else {
                return v;
            }
        }
    }

    Node* EraseWalk(Node* v, const T& el, bool& bad) {
        if (!v) {
            bad = true;
            return v;
        }
        if (IsEqual(v->value, el)) {
            if (v->size == 1) {
                return Kill(v);
            }
            if (v->left) {
                Node* lmax = GetMax(v->left);
                swap(lmax->value, v->value);
                return EraseWalk(lmax, el, bad);
            } else {
                Node* rmin = GetMin(v->right);
                swap(rmin->value, v->value);
                return EraseWalk(rmin, el, bad);
            }
        } else if (v->value < el) {
            return EraseWalk(v->right, el, bad);
        } else {
            return EraseWalk(v->left, el, bad);
        }
    }


public:
    class iterator {
    public:
        const T& operator*() const {
            return v_->value;
        }
        const T* operator->() const {
            return &(v_->value);
        }

        bool operator == (const iterator& rhs) const {
            return v_ == rhs.v_;
        }
        bool operator != (const iterator& rhs) const {
            return v_ != rhs.v_;
        }
        iterator& operator --() {
            if (!v_) {
                v_ = GetMax(root_);
                return *this;
            }
            if (v_->left) {
                v_ = GetMax(v_->left);
            } else {
                while (v_->parent) {
                    if (v_->parent->right == v_) {
                        v_ = v_->parent;
                        return *this;
                    }
                    v_ = v_->parent;
                }
            }
            return *this;
        }
        iterator& operator ++() {
            if (!v_) {
                return *this;
            }
            if (v_->right) {
                v_ = GetMin(v_->right);
            } else {
                while (v_) {
                    if (v_->parent && v_->parent->left == v_) {
                        v_ = v_->parent;
                        return *this;
                    }
                    v_ = v_->parent;
                }
            }
            return *this;
        }
        iterator operator ++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        iterator operator --(int) {
            auto temp = *this;
            --(*this);
            return temp;
        }
        iterator(): v_(nullptr), root_(nullptr) {}
        iterator(Node* v, Node* root): v_(v), root_(root) {}
        explicit iterator(Node* root): v_(nullptr), root_(root) {};
    private:
        Node* v_;
        Node* root_;
    };


    Set() {}

    size_t size() const {
        return GetSize(root_);
    }

    bool empty() const {
        return this->size() == 0;
    }

    void insert(const T& elem) {
        root_ = InsertWalk(root_, elem);
    }

    void erase(const T& elem) {
        bool bad = false;
        Node* v = EraseWalk(root_, elem, bad);
        if (!bad) {
            root_ = v;
        }
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }


    Set(std::initializer_list<T> elems) {
        for (const auto& el : elems) {
            insert(el);
        }
    }

    Set(const Set& rhs) {
        root_ = DfsCopy(rhs.root_);
    }
    Set& operator = (const Set& rhs) {
        if (this != &rhs) {
            DfsDelete(root_);
            root_ = nullptr;
            root_ = DfsCopy(rhs.root_);
        }
        return *this;
    }

    ~Set() {
        DfsDelete(root_);
    }



    iterator begin() const {
        return iterator(GetMin(root_), root_);
    }

    iterator end() const {
        return iterator(root_);
    }

    iterator find(const T& elem) const {
        Node* v = root_;
        while (v) {
            if (IsEqual(v->value, elem)) {
                break;
            }
            if (elem < v->value) {
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return iterator(v, root_);
    }

    iterator lower_bound(const T& elem) const {
        Node* v = root_;
        Node* res = nullptr;
        while (v) {
            if (!(v->value < elem)) {
                if (res == nullptr || v->value < res->value) {
                    res = v;
                }
                v = v->left;
            } else {
                v = v->right;
            }
        }
        return iterator(res, root_);
    }
private:
    Node* root_ = nullptr;
};
