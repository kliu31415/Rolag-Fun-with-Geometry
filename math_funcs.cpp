#include "math_funcs.h"
#include "sdl_base.h"
#include "fundamentals.h"
#include "game_map.h"
#include <cmath>
#include <algorithm>
void normToHypot(double &x, double &y, double z)
{
    if(std::fabs(x) < EPSILON) //the issue here is that x / h may equal inf. If moveX = inf, then units move off the map. Only happens in Debug build.
        x = 0;
    if(std::fabs(y) < EPSILON)
        y = 0;
    double h = std::hypot(x, y);
    if(h == 0)
    {
        //print_warning("Divide by 0");
        return;
    }
    double mult = z / h;
    x *= mult;
    y *= mult;
}
//general geometric object
GeometricObject::GeometricObject(double x, double y)
{
    angle = 0;
    this->x = x;
    this->y = y;
}
double GeometricObject::centerDist(const GeometricObject *other) const
{
    return std::hypot(x - other->x, y - other->y);
}
bool GeometricObject::collidesWithTerrain(const GameMap &game_map) const
{
    static Square tile(0, 0, 1); //this should work
    double sz = getSize() / 2;
    int minx = getX() - sz - 0.5, maxx = getX() + sz + 0.5;
    int miny = getY() - sz - 0.5, maxy = getY() + sz + 0.5;
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            if(!game_map.isPassableTile(i, j) && square(i - getX()) + square(j - getY()) < sz + std::sqrt(2))
            {
                tile.setX(i);
                tile.setY(j);
                if(collidesWith(&tile))
                    return true;
            }
        }
    }
    return false;
}
//point
Point::Point(double x, double y)
{
    this->x = x;
    this->y = y;
}
Point Point::getPos(double cenX, double cenY, double angle)
{
    double curAngle = std::atan2(y, x);
    double magnitude = std::hypot(x, y);
    return Point(cenX + magnitude * std::cos(curAngle + angle), cenY + magnitude * std::sin(curAngle + angle));
}
//circle
Circle::Circle(double x, double y, double diameter): GeometricObject(x, y)
{
    type = Type::circle;
    this->diameter = diameter;
}
Circle::Circle(const Circle *other): GeometricObject(other->getX(), other->getY())
{
    type = Type::circle;
    this->diameter = other->getSize();
}
double Circle::getSize() const
{
    return diameter;
}
void Circle::setSize(double size)
{
    diameter = size;
}
Circle::operator Polygon() const
{
    static Polygon res(10);
    const static double K = 1.0339; //approximating a circle as a decagon
    double sz = K * getSize() / 2;
    for(int i=0; i<10; i++)
    {
        res.vertices[i] = Point(sz * std::cos(i), sz * std::sin(i));
    }
    res.setSize(sz);
    res.setX(getX());
    res.setY(getY());
    return Polygon(res);
}
bool Circle::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::circle:
        return square(other->getX() - getX()) + square(other->getY() - getY()) < square(getSize()/2) + square(other->getSize()/2);
    case Type::square:
        return other->collidesWith(this);
    case Type::polygon:
        return other->collidesWith(this);
    default:
        print_error("GeometricObject has unknown type " + to_str((int)other->type));
    }
    return false; //just to avoid the compiler warning
}
//square
Square::Square(double x, double y, double size): GeometricObject(x, y)
{
    type = Type::square;
    this->size = size;
}
Square::Square(const Square *other): GeometricObject(other->getX(), other->getY())
{
    type = Type::square;
    this->size = other->getSize();
}
double Square::getSize() const
{
    return size;
}
void Square::setSize(double size)
{
    this->size = size;
}
Square::operator Polygon() const
{
    static Polygon res(4);
    double sz = getSize() / 2;
    res.vertices[0] = Point(sz, sz);
    res.vertices[1] = Point(sz, -sz);
    res.vertices[2] = Point(-sz, -sz);
    res.vertices[3] = Point(-sz, sz);
    res.setSize(sz);
    res.setX(getX());
    res.setY(getY());
    return Polygon(res);
}
static inline bool isPointInCircle(double pointX, double pointY, double x, double y, double r)
{
    return pow(pointX - x, 2) + pow(pointY - y, 2) < r*r;
}
static inline bool isPointInRect(double pointX, double pointY, double x, double y, double w, double h)
{
    return pointX > x && pointX < x+w && pointY > y && pointY < y+h;
}
static inline bool isPointInSquare(double pointX, double pointY, double x, double y, int s)
{
    return pointX > x && pointX < x+s && pointY > y && pointY < y+s;
}
static inline bool squaresIntersect(double x1, double y1, double s1, double x2, double y2, double s2)
{
    return ((x1>x2 && x1<x2+s2) || (x2>x1 && x2<x1+s1)) && ((y1>y2 && y1<y2+s2) || (y2>y1 && y2<y1+s1));
}
bool Square::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::circle:
        return isPointInCircle(other->getX(), other->getY(), getX() - getSize()/2, getY() - getSize()/2, other->getSize()/2) ||
               isPointInCircle(other->getX(), other->getY(), getX() + getSize()/2, getY() - getSize()/2, other->getSize()/2) ||
               isPointInCircle(other->getX(), other->getY(), getX() - getSize()/2, getY() + getSize()/2, other->getSize()/2) ||
               isPointInCircle(other->getX(), other->getY(), getX() + getSize()/2, getY() + getSize()/2, other->getSize()/2) ||
               isPointInRect(other->getX(), other->getY(), getX() - getSize()/2 - other->getSize()/2,
                             getY() - getSize()/2, getSize() + other->getSize(), getSize()) ||
               isPointInRect(other->getX(), other->getY(), getX() - getSize()/2,
                             getY() - getSize()/2 - other->getSize()/2, getSize(), getSize() + other->getSize());
    case Type::square:
        return squaresIntersect(getX() - getSize()/2, getY() - getSize()/2, getSize(),
                               other->getX() - other->getSize()/2, other->getY() - other->getSize()/2, other->getSize());
    case Type::polygon:
        return other->collidesWith(this);
    default:
        print_error("GeometricObject has unknown type " + to_str((int)other->type));
    }
    return false; //just to avoid the compiler warning
}
//polygon
Polygon::Polygon(int numVertices)
{
    type = Type::polygon;
    setAngle(0);
    vertices = static_array<Point>(numVertices);
}
Polygon::Polygon(double x, double y, std::initializer_list<Point> points): GeometricObject(x, y)
{
    type = Type::polygon;
    setAngle(0);
    vertices.getDeepCopy(points);
    computeSize();
}
Polygon::Polygon(double x, double y, static_array<Point> &points): GeometricObject(x, y)
{
    type = Type::polygon;
    setAngle(0);
    vertices.getDeepCopy(points);
    computeSize();
}
Polygon::Polygon(const Polygon &other): GeometricObject(other.getX(), other.getY())
{
    type = Type::polygon;
    setAngle(0);
    setX(other.getX());
    setY(other.getY());
    vertices.getDeepCopy(other.vertices);
    computeSize();
}
void Polygon::computeSize()
{
    size = 0;
    for(size_t i=0; i<vertices.size(); i++)
    {
        for(size_t j=0; j<i; j++)
        {
            size = std::max(size, std::hypot(vertices[i].x - vertices[j].x, vertices[i].y - vertices[j].y));
        }
    }
}
size_t Polygon::getNumVertices()
{
    return vertices.size();
}
void Polygon::setSize(double size)
{
    this->size = size;
}
double Polygon::getSize() const
{
    return size;
}
Polygon::operator Polygon() const
{
    print_warning("Polygon::operator Polygon() should never be called");
    return Polygon(*this); //avoid the compiler warning and if we ever actually want to do it note we have to do a deepcopy because of the unique_ptr
}
static inline double cp(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}
//WARNING: MAY BE UNPORTABLE!!
#define TO_ULL(x) (*reinterpret_cast<unsigned long long*>(&x))
static inline bool segmentsIntersect(Point a1, Point a2, Point b1, Point b2)
{
    //points are implicitly converted to 2D geometric vectors
    //might not work for some edge cases that can probably be ignored because they will have a minimal effect due to the nature of floating points
    double x1 = a1.x - b1.x, x2 = b2.x - a1.x, x3 = a2.x - b2.x, x4 = b1.x - a2.x;
    double y1 = a1.y - b1.y, y2 = b2.y - a1.y, y3 = a2.y - b2.y, y4 = b1.y - a2.y;
    //return sign(cp(x1, y1, x2, y2)) == sign(cp(x3, y3, x4, y4)) && sign(cp(x2, y2, x3, y3)) == sign(cp(x4, y4, x1, y1));
    double c1 = cp(x1, y1, x2, y2);
    double c2 = cp(x3, y3, x4, y4);
    double c3 = cp(x2, y2, x3, y3);
    double c4 = cp(x4, y4, x1, y1);
    //return sign(c1)==sign(c2) && sign(c3)==sign(c4);
    return ((1ull<<63) & ((TO_ULL(c1) ^ ~TO_ULL(c2)) & (TO_ULL(c3) ^ ~TO_ULL(c4))));
}
bool Polygon::collidesWithPolygon(const Polygon &other) const
{
    //check if any edges intersect
    static static_array<Point> v1(11), v2(11); //allocating it every time would be costly. Assume that no polygon has more than 10 vertices.
    for(size_t i=0; i<vertices.size(); i++) //vertices.get() is used as a read only operation to access a Point
    {
        v1[i] = vertices.get(i).getPos(getX(), getY(), getAngle());
    }
    v1[vertices.size()] = v1.get(0);
    for(size_t i=0; i<other.vertices.size(); i++)
    {
        v2[i] = other.vertices.get(i).getPos(other.getX(), other.getY(), other.getAngle());
    }
    v2[other.vertices.size()] = v2.get(0);
    for(size_t i=0; i<vertices.size(); i++) //they intersect if any pair of edges intersect
    {
        for(size_t j=0; j<other.vertices.size(); j++)
        {
            if(segmentsIntersect(v1.get(i), v1.get(i+1), v2.get(j), v2.get(j+1)))
            {
                /*cout << v1[i].x << " " << v1[i].y << " " << v1[i+1].x << " " << v1[i+1].y << endl;
                cout << v2[j].x << " " << v2[j].y << " " << v2[j+1].x << " " << v2[j+1].y << endl;
                exit(EXIT_SUCCESS);*/
                return true;
            }
        }
    }
    return false;
    //also check if one is inside the other with ray tracing
    int intersectionCount = 0;
    Point a = v2.get(0);
    Point b(a.x + 1e9, a.y + 1); //the slope of 1e-9 makes it probable that they ray won't cross any vertices
    for(size_t i=0; i<vertices.size(); i++)
    {
        if(segmentsIntersect(a, b, v1.get(i), v1.get(i+1)))
            intersectionCount++;
    }
    return intersectionCount%2 == 1;
}
bool Polygon::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::square:
    {
            Polygon p = (Polygon)(*other);
            return collidesWithPolygon(p);
        }
        break;
    case Type::circle:
        {
            Polygon p = (Polygon)(*other);
            return collidesWithPolygon(p);
        }
        break;
    case Type::polygon:
        return collidesWithPolygon(*static_cast<const Polygon*>(other));
    default:
        print_error("GeometricObject has unknown type " + to_str((int)other->type));
    }
    return false; //just to avoid the compiler warning
}
