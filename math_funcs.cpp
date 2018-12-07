#include "math_funcs.h"
#include "sdl_base.h"
#include "fundamentals.h"
#include "game_map.h"
#include "basic_game_funcs.h"
#include <cmath>
#include <algorithm>
void __attribute__((hot)) normToHypot(double &x, double &y, double z)
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
bool GeometricObject::collidesWithTerrain(const GenericMapRoom &room) const
{
    double sz = getRadius();
    int minx = getX() - sz - 0.5, maxx = getX() + sz + 0.5;
    int miny = getY() - sz - 0.5, maxy = getY() + sz + 0.5;
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            if(!room.isPassableTile(i, j))
            {
                Square tile(i, j, 1);
                if(collidesWith(&tile))
                    return true;
            }
        }
    }
    return false;
}
bool GeometricObject::collidesWithUnflyableTile(const GenericMapRoom &room) const
{
    double sz = getRadius();
    int minx = getX() - sz - 0.5, maxx = getX() + sz + 0.5;
    int miny = getY() - sz - 0.5, maxy = getY() + sz + 0.5;
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            if(!room.isFlyableTile(i, j))
            {
                Square tile(i, j, 1);
                if(collidesWith(&tile))
                    return true;
            }
        }
    }
    return false;
}
bool GeometricObject::collidesWithTile(const GenericMapRoom &room, MapTile type) const
{
    static Square tile(0, 0, 1); //might save a little time by making it static
    double sz = getRadius();
    int minx = getX() - sz - 0.5, maxx = getX() + sz + 0.5;
    int miny = getY() - sz - 0.5, maxy = getY() + sz + 0.5;
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            if(i>=0 && j>=0 && i<room.getNumColumns() && j<room.getNumRows() && room.tiles[i][j] == type)
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
bool GeometricObject::handleCollisionsWithCollapsingTiles(GameMap &game_map) const
{
    static Square tile(0, 0, 1); //might save a little time by making it static
    double sz = getRadius();
    int minx = getX() - sz - 0.5, maxx = getX() + sz + 0.5;
    int miny = getY() - sz - 0.5, maxy = getY() + sz + 0.5;
    for(int i=minx; i<=maxx; i++)
    {
        for(int j=miny; j<=maxy; j++)
        {
            if(i>=0 && j>=0 && i<game_map.getNumColumns() && j<game_map.getNumRows() && game_map.timeTillCollapse[i][j]!=NOT_SET)
            {
                tile.setX(i);
                tile.setY(j);
                if(collidesWith(&tile))
                {
                    game_map.timeTillCollapse[i][j] -= TICK_SPEED * (1 + (DIFFICULTY-1)/5);
                    if(game_map.timeTillCollapse[i][j] <= 0)
                        game_map.tiles[i][j] = MapTile::hurt;
                }
            }
        }
    }
    return false;
}
inline int conv(double pos, double cameraPos, int ppt)
{
    return (pos - cameraPos) * ppt;
}
void GeometricObject::drawHitbox(double cameraX, double cameraY) const
{
    Polygon p = Polygon(*this);
    int ppt = getPixelsPerTile();
    static static_array<Point> points(MAX_VERT_IN_POLYGON + 1);
    for(int i=0; i<(int)p.vertices.size(); i++)
    {
        points[i] = p.vertices.get(i).getPos(getX(), getY(), getAngle());
        points[i].x = conv(points[i].x, cameraX, ppt);
        points[i].y = conv(points[i].y, cameraY, ppt);
    }
    points[p.vertices.size()] = points[0];
    for(int i=0; i<(int)p.vertices.size(); i++)
        drawLine(points[i].x, points[i].y, points[i+1].x, points[i+1].y, 0, 0, 255, 200);
}
//point
__attribute__((hot)) Point::Point(double x, double y)
{
    this->x = x;
    this->y = y;
}
Point __attribute__((hot)) Point::getPos(double cenX, double cenY, double angle)
{
    double curAngle = std::atan2(y, x);
    double magnitude = std::hypot(x, y);
    return Point(cenX + magnitude * std::cos(curAngle + angle), cenY + magnitude * std::sin(curAngle + angle));
}
//circle
Circle::Circle(double x, double y, double radius): GeometricObject(x, y)
{
    type = Type::circle;
    this->radius = radius;
}
Circle::Circle(const Circle *other): GeometricObject(other->getX(), other->getY())
{
    type = Type::circle;
    this->radius = other->getRadius();
}
__attribute__((hot)) double Circle::getRadius() const
{
    return radius;
}
void Circle::setRadius(double size)
{
    radius = size;
}
__attribute__((hot)) Circle::operator Polygon() const
{
    Polygon res(10);
    const static double K = 1.0339; //approximating a circle as a decagon
    double sz = K * getRadius();
    for(int i=0; i<10; i++)
    {
        res.vertices[i] = Point(sz * std::cos(i), sz * std::sin(i));
    }
    res.setRadius(sz);
    res.setX(getX());
    res.setY(getY());
    return res;
}
bool Circle::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::circle:
        return square(other->getX() - getX()) + square(other->getY() - getY()) < square(getRadius()) + square(other->getRadius());
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
    this->size = other->size;
}
double __attribute__((hot)) Square::getRadius() const
{
    return size * sqrt(0.5);
}
void Square::setSideLength(double size)
{
    this->size = size;
}
__attribute__((hot)) Square::operator Polygon() const
{
    Polygon res(4);
    double sz = size / 2;
    res.vertices[0] = Point(sz, sz);
    res.vertices[1] = Point(sz, -sz);
    res.vertices[2] = Point(-sz, -sz);
    res.vertices[3] = Point(-sz, sz);
    res.setRadius(getRadius());
    res.setX(getX());
    res.setY(getY());
    return res;
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
bool __attribute__((hot)) Square::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::circle:
        return isPointInCircle(other->getX(), other->getY(), getX() - size/2, getY() - size/2, other->getRadius()) ||
               isPointInCircle(other->getX(), other->getY(), getX() + size/2, getY() - size/2, other->getRadius()) ||
               isPointInCircle(other->getX(), other->getY(), getX() - size/2, getY() + size/2, other->getRadius()) ||
               isPointInCircle(other->getX(), other->getY(), getX() + size/2, getY() + size/2, other->getRadius()) ||
               isPointInRect(other->getX(), other->getY(), getX() - size/2 - other->getRadius(),
                             getY() - size/2, size + other->getRadius()*2, size) ||
               isPointInRect(other->getX(), other->getY(), getX() - size/2,
                             getY() - size/2 - other->getRadius(), size, size + other->getRadius()*2);
    case Type::square:
        {
            auto *o = static_cast<const Square*>(other);
            return squaresIntersect(getX() - size/2, getY() - size/2, size,
                               other->getX() - o->size/2, other->getY() - o->size/2, o->size);
        }
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
    vertices = static_array<Point>(numVertices);
    setAngle(0);
}
Polygon::Polygon(double x, double y, std::initializer_list<Point> points): GeometricObject(x, y)
{
    type = Type::polygon;
    setAngle(0);
    vertices = points;
    computeRadius();
}
Polygon::Polygon(double x, double y, static_array<Point> &points): GeometricObject(x, y)
{
    type = Type::polygon;
    setAngle(0);
    vertices = points;
    computeRadius();
}
__attribute__((hot)) Polygon::Polygon(const Polygon &other): GeometricObject(other.getX(), other.getY())
{
    type = Type::polygon;
    setAngle(0);
    setX(other.getX());
    setY(other.getY());
    setRadius(other.radius);
    vertices = other.vertices;
}
void __attribute__((hot)) Polygon::computeRadius()
{
    radius = 0;
    for(auto &i: vertices)
    {
        radius = std::max(radius, std::hypot(i.x, i.y));
    }
}
size_t Polygon::getNumVertices()
{
    return vertices.size();
}
void __attribute__((hot)) Polygon::setRadius(double radius)
{
    this->radius = radius;
}
double __attribute__((hot)) Polygon::getRadius() const
{
    return radius;
}
Polygon::operator Polygon() const
{
    return *this;
}
static inline __attribute__((hot)) double cp(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}
//CODE_WARNING: may be unportable
#define TO_ULL(x) (*reinterpret_cast<unsigned long long*>(&x))
static inline bool __attribute__((hot)) segmentsIntersect(Point a1, Point a2, Point b1, Point b2)
{
    //points are implicitly converted to 2D geometric vectors
    //might not work for some edge cases which can probably be ignored because they will have a minimal effect due to the nature of floating points
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
bool __attribute__((hot)) Polygon::collidesWithPolygon(const Polygon &other) const
{
    if(square(getRadius() + other.getRadius()) < square(getX() - other.getX()) + square(getY() - other.getY())) //bounding circles don't overlap
        return false;
    //check if any edges intersect
    static static_array<Point> v1(MAX_VERT_IN_POLYGON+1), v2(MAX_VERT_IN_POLYGON+1); //allocating it every time would be costly. Assume that no polygon has more than 12 vertices.
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
    //also check if one is inside the other with ray tracing
    return false;
    //CODE_WARNING: it's assumed that one polygon will never be inside another (which is true if time is simulated continuously)
    //disabling ray tracing saves a bit of CPU but could cause issues with extremely small, fast things
    /*int intersectionCount = 0;
    Point a = v2.get(0);
    Point b(a.x + INF, a.y + 1); //the slope of 1e-9 makes it probable that the ray won't cross any vertices
    for(size_t i=0; i<vertices.size(); i++)
    {
        if(segmentsIntersect(a, b, v1.get(i), v1.get(i+1)))
            intersectionCount++;
    }
    if(intersectionCount%2 == 1)
        return true;
    intersectionCount = 0;
    a = v1.get(0);
    b = Point(b.x + INF, b.y + 1);
    for(size_t i=0; i<other.vertices.size(); i++)
    {
        if(segmentsIntersect(a, b, v2.get(i), v2.get(i+1)))
            intersectionCount++;
    }
    return intersectionCount%2 == 1;*/
}
bool __attribute__((hot)) Polygon::collidesWith(const GeometricObject *other) const
{
    switch(other->type)
    {
    case Type::square:
        {
            /*println("OK");
            println(to_str(other->getRadius()));*/
            Polygon p = (Polygon)(*other);
            return collidesWithPolygon(p);
        }
        break;
    case Type::circle:
        {
            /*println("OK");
            println(to_str(other->getRadius()));*/
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
//other funcs
std::string to_hex_len8(unsigned v)
{
    std::string res = "0x";
    for(int i=28; i>=0; i-=4)
    {
        int t = ((v>>i) & 0xf);
        if(t < 10)
            res += '0' + t;
        else res += 'A' + t-10;
    }
    return res;
}
std::string to_hex(unsigned v)
{
    std::string res;
    while(v > 0)
    {
        int t = v & 0xf;
        if(t < 10)
            res += '0' + t;
        else res += 'A' + t-10;
        v >>= 4;
    }
    res += "x0";
    std::reverse(res.begin(), res.end());
    return res;
}
void flip(double &v)
{
    if(v == 0)
        v = 1;
    else v = 0;
}
