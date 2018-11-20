#pragma once
#include <string>
#include <memory>
#include <initializer_list>
#include "fundamentals.h"
class GameMap;
struct Polygon;
class GeometricObject
{
    double x, y;
protected:
    GeometricObject() = default;
    GeometricObject(double x, double y);
public:
    enum class Type{circle, square, polygon} type;
    virtual bool collidesWith(const GeometricObject *other) const = 0;
    bool collidesWithTerrain(const GameMap &game_map) const;
    virtual double getSize() const = 0; //returns the maximum width/length of this object
    virtual void setSize(double size) = 0;
    inline double getX() const {return x;}
    inline double getY() const {return y;}
    inline void setX(double x) {this->x = x;}
    inline void setY(double y) {this->y = y;}
    double centerDist(const GeometricObject *other) const;
    virtual operator Polygon() const = 0;
};

class Circle: public GeometricObject //cannot be rotated
{
    double diameter;
public:
    Circle(const Circle*);
    Circle(double x, double y, double diameter);
    bool collidesWith(const GeometricObject *other) const override;
    double getSize() const override;
    void setSize(double size) override;
    operator Polygon() const override;
};

class Square: public GeometricObject //cannot be rotated
{
    double size;
public:
    Square(const Square*);
    Square(double x, double y, double size);
    bool collidesWith(const GeometricObject *other) const override;
    double getSize() const override;
    void setSize(double size) override;
    operator Polygon() const override;
};

struct Point
{
    double x, y;
    Point() = default; //this really shouldn't be used. it has to be here to stop the compiler from complaining
    Point(double x, double y);
    Point getPos(double cenX, double cenY, double angle); //rotates about the origin, which is assumed to be the center, and then translates by (cenX, cenY)
};

class Polygon: public GeometricObject //a polygon consists of a list of vertices and can be rotated
{
    double angle, size;
public:
    static_array<Point> vertices;
    Polygon(double x, double y, std::initializer_list<Point> points);
    Polygon(double x, double y, static_array<Point> &points);
    Polygon(const Polygon &other);
    Polygon(int numVertices);
    size_t getNumVertices();
    bool collidesWith(const GeometricObject *other) const override;
    bool collidesWithPolygon(const Polygon &other) const;
    double getSize() const override;
    void setSize(double size) override;
    operator Polygon() const override;
    void computeSize();
};

template<class T> inline T sign(T v)
{
    if(v < 0)
        return -1;
    return v > 0;
}
template<class T> inline T square(T v)
{
    return v*v;
}
void normToHypot(double &x, double &y, double z);
