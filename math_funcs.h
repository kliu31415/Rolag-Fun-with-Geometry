#pragma once
#include <string>
#include <memory>
#include <initializer_list>
#include "fundamentals.h"
const static double EPSILON = 1e-10;
const int MAX_VERT_IN_POLYGON = 12; //IMPORTANT! You must change this if you want polygons with more vertices or the program will crash!!
class GameMap;
class GenericMapRoom;
enum class MapTile: int;
struct Polygon;
class GeometricObject
{
    double x, y;
    double angle; //NOTE: Square and Circle are irrotational and this should not be touched for those objects
protected:
    GeometricObject() = default;
    GeometricObject(double x, double y);
public:
    enum class Type{circle, square, polygon} type;
    virtual bool collidesWith(const GeometricObject *other) const = 0;
    bool collidesWithTerrain(const GenericMapRoom &room) const;
    bool collidesWithUnflyableTile(const GenericMapRoom &room) const;
    bool collidesWithTile(const GenericMapRoom &room, MapTile type) const;
    bool handleCollisionsWithCollapsingTiles(GameMap &game_map) const;
    virtual double getRadius() const = 0; //returns the maximum distance from the center of rotation to any vertex
    double __attribute__((always_inline)) getX() const {return x;}
    double __attribute__((always_inline)) getY() const {return y;}
    void __attribute__((always_inline)) setX(double x) {this->x = x;}
    void __attribute__((always_inline)) setY(double y) {this->y = y;}
    double __attribute__((always_inline)) getAngle() const {return angle;}
    void __attribute__((always_inline)) setAngle(double angle) {this->angle = angle;}
    double centerDist(const GeometricObject *other) const;
    virtual operator Polygon() const = 0;
    void drawHitbox(double cameraX, double cameraY) const;
};

class Circle: public GeometricObject //cannot be rotated
{
    double radius;
public:
    Circle(const Circle*);
    Circle(double x, double y, double radius);
    bool collidesWith(const GeometricObject *other) const override;
    double getRadius() const override;
    void setRadius(double size);
    operator Polygon() const override;
};

class Square: public GeometricObject //cannot be rotated
{
    double size;
public:
    Square(const Square*);
    Square(double x, double y, double size);
    bool collidesWith(const GeometricObject *other) const override;
    double getRadius() const override;
    void setSideLength(double size);
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
    double radius;
public:
    static_array<Point> vertices;
    Polygon(double x, double y, std::initializer_list<Point> points);
    Polygon(double x, double y, static_array<Point> &points);
    Polygon(const Polygon &other);
    Polygon(int numVertices);
    size_t getNumVertices();
    bool collidesWith(const GeometricObject *other) const override;
    bool collidesWithPolygon(const Polygon &other) const;
    double getRadius() const override;
    void setRadius(double radius);
    operator Polygon() const override;
    void computeRadius();
};
template<class T> inline __attribute__((always_inline)) T sign(T v)
{
    if(v < 0)
        return -1;
    return v > 0;
}
template<class T> inline __attribute__((always_inline)) T square(T v)
{
    return v*v;
}
void normToHypot(double &x, double &y, double z);
std::string to_hex(unsigned v);
std::string to_hex(int v) = delete;
std::string to_hex_len8(unsigned v);
std::string to_hex_len8(int v) = delete;
void flip(double &v);
