#include "fundamentals.h"
#include <numeric>
#include <algorithm>
#include <string>
#ifdef __GNUC__
namespace std
{
    size_t hash<pii>::operator()(const pii &v) const
    {
        return hash<int>()(v.A) + 17 * hash<int>()(v.B);
    }
}
#endif // __GNUC__
DisjointSet::DisjointSet(int sz)
{
    this->sz = sz;
    p.resize(sz);
    d.resize(sz);
    std::iota(p.begin(), p.end(), 0);
    std::fill(d.begin(), d.end(), 0);
}
int DisjointSet::size() const
{
    return sz;
}
void DisjointSet::combine(int a, int b)
{
    a = getp(a);
    b = getp(b);
    if(a == b)
        return;
    if(d[a] < d[b])
        p[a] = b;
    else if(d[b] < d[a])
        p[b] = a;
    else
    {
        p[a] = b;
        d[b]++;
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
