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
#include <ext/pb_ds/assoc_container.hpp> //hmm I could use custom_hash_set too but whatever they're around the same speed for normal use
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
    static_array(static_array &&other)
    {
        vals = std::move(other.vals);
        sz = other.sz;
    }
    static_array(std::initializer_list<T> x)
    {
        this->sz = x.size();
        vals = std::make_unique<T[]>(sz);
        int pos = 0;
        for(auto &i: x)
            vals[pos++] = i;
    }
    static_array &operator = (static_array &&other)
    {
        vals = std::move(other.vals);
        sz = other.sz;
        return *this;
    }
    static_array &operator = (const static_array &other)
    {
        sz = other.sz;
        vals = std::make_unique<T[]>(sz);
        for(size_t i=0; i<sz; i++)
            vals[i] = other.vals[i];
        return *this;
    }
    __attribute__((always_inline)) T &operator[](size_t idx)
    {
        return vals[idx];
    }
    __attribute__((always_inline)) T get(size_t idx) const
    {
        return vals[idx];
    }
    __attribute__((always_inline)) size_t size() const
    {
        return sz;
    }
    T *begin()
    {
        return vals.get();
    }
    T *end()
    {
        return vals.get() + sz;
    }
};
template<class T> class custom_hash_set
{
    size_t cur_size, max_size, (*hfunc)(T);
    enum possible_states: uint8_t{dead = 0, filled = 1, empty_ = 2};
    uint8_t *state;
    T *x;
    bool is_prime(size_t x)
    {
        for(size_t i=2; i*i<=x; i++)
            if(x % i == 0)
                return false;
        return true;
    }
    void resize(size_t new_size) //if new_size is less than cur_size, then this fails
    {
        //prime mods are best
        while(!is_prime(new_size))
            new_size++;
        custom_hash_set nxt(new_size, hfunc);
        for(size_t i=0; i<max_size; i++)
            if(state[i] == filled)
                nxt.insert(x[i]);
        max_size = new_size;
        //use swap because the destructor for nxt will be called
        std::swap(x, nxt.x);
        std::swap(state, nxt.state);
    }
public:
    custom_hash_set(int max_size = 3, size_t (*hfunc)(T) = [](T val) -> size_t {std::hash<T> hfunc; return hfunc(val);})
    {
        this->hfunc = hfunc;
        while(!is_prime(max_size))
            max_size++;
        this->max_size = max_size;
        this->cur_size = 0;
        state = new uint8_t[max_size];
        x = new T[max_size];
        std::fill(state, state+max_size, empty_);
    }
    ~custom_hash_set()
    {
        delete state;
        delete x;
    }
    bool erase(const T &v) //warning: endless loop if the hash set is full (which it hopefully shouldn't be due to resizing)
    {
        if((cur_size + 1) * 27 <= max_size)
        {
            resize(max_size / 3);
        }
        size_t pos = hfunc(v) % max_size;
        while(true)
        {
            if(state[pos] == empty_)
                return false;
            if(state[pos]==filled && x[pos]==v)
            {
                cur_size--;
                state[pos] = dead;
                return true;
            }
            pos++;
            if(pos >= max_size)
                pos = 0;
        }
    }
    bool insert(const T &v)
    {
        if(cur_size * 3 >= max_size)
        {
            resize(3 * max_size);
        }
        size_t pos = hfunc(v) % max_size;
        size_t first = max_size;
        while(true)
        {
            if(state[pos] != filled)
            {
                if(state[pos] == empty_)
                {
                    cur_size++;
                    if(first == max_size)
                        first = pos;
                    state[first] = filled;
                    x[first] = v;
                    return true;
                }
                else if(first == max_size)
                    first = pos;
            }
            else if(x[pos] == v)
                return false;
            pos++;
            if(pos >= max_size)
                pos = 0;
        }
    }
    size_t count(const T &v) const
    {
        size_t pos = hfunc(v) % max_size;
        while(true)
        {
            if(state[pos] == empty_)
                return 0;
            if(state[pos]==filled && x[pos]==v)
                return 1;
            pos++;
            if(pos >= max_size)
                pos = 0;
        }
    }
    T *find(const T &v) const
    {
        size_t pos = hfunc(v) % max_size;
        while(true)
        {
            if(state[pos] == empty_)
                return NULL;
            else if(state[pos]==filled && x[pos]==v)
                return &x[pos];
            pos++;
            if(pos >= max_size)
                pos = 0;
        }
    }
    T &get_random_element() const
    {
        if(cur_size == 0)
            throw std::runtime_error("cannot get random element when there are 0 elements");
        while(true)
        {
            size_t pos = randuz() % max_size;
            if(state[pos] == filled)
                return x[pos];
        }
    }
    T &get_random_element_uneven() const //faster but not truly random when used more than once
    {
        if(cur_size == 0)
            throw std::runtime_error("cannot get random element when there are 0 elements");
        size_t pos = randuz() % max_size;
        while(true)
        {
            if(state[pos] == filled)
                return x[pos];
            pos++;
            if(pos >= max_size)
                pos = 0;
        }
    }
    void clear()
    {
        std::fill(state, state+max_size, empty_);
    }
    size_t capacity()
    {
        return max_size;
    }
    size_t size()
    {
        return cur_size;
    }
};
class DisjointSet
{
    static_array<int> p, s;
public:
    DisjointSet(int sz);
    void combine(int a, int b);
    int getp(int x);
    int size_of_component(int x);
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
