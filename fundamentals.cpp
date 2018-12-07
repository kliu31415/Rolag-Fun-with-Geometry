#include "fundamentals.h"
#include <numeric>
#include <algorithm>
#include <string>
namespace std
{
    size_t hash<pii>::operator()(const pii &v) const
    {
        return hash<int>()(v.A) + 17 * hash<int>()(v.B);
    }
}
DisjointSet::DisjointSet(int sz)
{
    p = static_array<int>(sz);
    s = static_array<int>(sz);
    std::iota(p.begin(), p.end(), 0);
    std::fill(s.begin(), s.end(), 1);
}
int DisjointSet::size() const
{
    return p.size();
}
int DisjointSet::size_of_component(int x)
{
    return s[getp(x)];
}
void DisjointSet::combine(int a, int b)
{
    a = getp(a);
    b = getp(b);
    if(a == b)
        return;
    if(s[a] < s[b])
    {
        p[a] = b;
        s[b] += s[a];
    }
    else
    {
        p[b] = a;
        s[a] += s[b];
    }
}
int DisjointSet::getp(int x)
{
    if(x == p[x])
        return x;
    p[x] = getp(p[x]);
    return p[x];
}
static std::string clipFilePath(std::string &path)
{
    for(int i=path.size()-1; i>=0; i--)
        if(path[i]=='\\' || path[i]=='/')
            return path.substr(i+1);
    return path;
}
void _print_warning(std::string s, std::string file, int line, std::string func)
{
    println("Warning (file=" + clipFilePath(file) + ", line=" + to_str(line) + ", func=" + to_str(func) + "): " + s);
}
void _print_error(std::string s, std::string file, int line, std::string func)
{
    println("Error (file=" + clipFilePath(file) + ", line=" + to_str(line) + ", func=" + to_str(func) + "): " + s);
}
