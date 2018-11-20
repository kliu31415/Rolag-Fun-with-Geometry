#pragma once
#include <iostream>
#include <cassert>
#include <utility>
#include <vector>
#include <string>
#include <functional>
#include "sdl_base.h"
#define pii std::pair<int, int>
#define mp std::make_pair
#define A first
#define B second
#define INF ((int)(1e9 + 23000))
#define NOT_SET ((int)(1e9 + 23001))
#define PI 3.14159265358979323
#define TO_DEG(x) (x * 180 / PI)
#define TO_RAD(x) (x * PI / 180)
typedef long long ll;
using std::cout;
using std::endl;
#ifdef __GNUC__
#include <ext/pb_ds/assoc_container.hpp>
using __gnu_pbds::gp_hash_table;
using __gnu_pbds::null_type;
template<class T, class hashFunc = std::hash<T> >
struct fast_set: public gp_hash_table<T, null_type, hashFunc>
{
    int count(T v) const
    {
        return this->find(v) != this->end();
    }
};
template<class T1, class T2, class hashFunc = std::hash<T1> >
struct fast_map: public gp_hash_table<T1, T2, hashFunc>
{
    int count(T1 v) const
    {
        return this->find(v) != this->end();
    }
};
namespace std
{
    template<> struct hash<pii>
    {
        size_t operator()(const pii &v) const;
    };
}
#else
#include <map>
#include <set>
#define fast_map std::map
#define fast_set std::set
#endif // __GNUC__
template<class T> class static_array //dynamically allocated, but cannot be resized like a vector
{
    std::unique_ptr<T[]> vals;
    size_t sz;
public:
    static_array() = default;
    static_array(size_t sz)
    {
        this->sz = sz;
        vals = std::make_unique<T[]>(sz);
    }
    static_array(std::initializer_list<T> x)
    {
        this->sz = x.size();
        vals = std::make_unique<T[]>(sz);
        int pos = 0;
        for(auto &i: x)
            vals[pos++] = i;
    }
    void getDeepCopy(const static_array<T> &other)
    {
        this->sz = other.sz;
        this->vals = std::make_unique<T[]>(sz);
        for(size_t i=0; i<sz; i++)
            vals[i] = other.vals[i];
    }
    inline T &operator[](size_t idx)
    {
        return vals[idx];
    }
    inline T get(size_t idx) const
    {
        return vals[idx];
    }
    inline size_t size() const
    {
        return sz;
    }
    T* begin()
    {
        return vals.get();
    }
    T* end()
    {
        return vals.get() + sz;
    }
};
struct DisjointSet
{
    int sz;
    std::vector<int> p, d;
    DisjointSet(int sz);
    void combine(int a, int b);
    int getp(int x);
    int size() const;
};
void _print_warning(std::string s, std::string file, int line, std::string func);
void _print_error(std::string s, std::string file, int line, std::string func);
#ifndef NDEBUG
#define print_warning(x) _print_warning(x, __FILE__, __LINE__, __func__)
#define print_error(x) _print_error(x, __FILE__, __LINE__, __func__)
#else
#define print_warning(x) ((void)(0))
#define print_error(x) ((void)(0))
#endif //NDEBUG
