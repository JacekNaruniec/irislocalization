#pragma once

#include <iostream>

/// Simple template 2D point class
template <class T>
class SimplePoint
{
public:
	T x, y;

	SimplePoint() : x((T)0.0), y((T)0.0) {};

	SimplePoint(T _x, T _y) : x(_x), y(_y){};

	SimplePoint(const SimplePoint& p) : x(static_cast<T>(p.x)), y(static_cast<T>(p.y)){};

	SimplePoint& operator +=(SimplePoint<int> &p)
	{
		x += (T)p.x; y += (T)p.y; return *this;
	}

	SimplePoint& operator /=(T s)
	{
		x /= s; y /= s; return *this;
	}

	SimplePoint& operator *=(T s)
	{
		x *= s; y *= s; return *this;
	}

	SimplePoint<int> operator *(T s)
	{
		SimplePoint<int> p_r = *this;
		p_r.x *= s; p_r.y *= s;
		return p_r;
	}

	SimplePoint<int> operator -(SimplePoint<int> &p)
	{
		SimplePoint<int> p_r;
		p_r.x = x - p.x; p_r.y = y - p.y;
		return p_r;
	}

	SimplePoint<int> operator +(SimplePoint<int> &p)
	{
		SimplePoint<int> p_r;
		p_r.x = x + p.x; p_r.y = y + p.y;
		return p_r;
	}

	SimplePoint& operator=(const SimplePoint& other_point)
	{
		x = other_point.x;
		y = other_point.y;
		return *this;
	}

	/// Calculates euclidean distance of this point to the given point.
	template <class T2>
	T distance(const SimplePoint<T2>& toCompare) const
	{
		return sqrtf(powf(x - toCompare.x, 2.0f) + powf(y - toCompare.y, 2.0f));
	}

	/// Setting proper point values.
	void set(float _x, float _y)
	{
		x = _x; y = _y;
	}

	friend std::ostream& operator<<(std::ostream& out, const SimplePoint& p)
	{
		out << "(" << p.x << ", " << p.y << ")";
		return out;
	}
};
