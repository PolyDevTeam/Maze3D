#ifndef __POS_HPP__
#define __POS_HPP__

class Pos {
public:
    Pos();
    Pos(float x, float y, float z);
    Pos(const Pos& pos);
    ~Pos();

    void setX(float x);
    void setY(float y);
    void setZ(float z);
    float getX() const;
    float getY() const;
    float getZ() const;
private:
    float x;
    float y;
    float z;
};

#endif /* __POS_HPP__ */
