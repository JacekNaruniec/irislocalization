#pragma once

#include "simplepoint.h"
#include <algorithm>

class SimpleRect
{
public:
	SimpleRect() {}

	SimpleRect(int _left, int _top, int _width, int _height)
	{
		width = _width;
		height = _height;
		left = _left;
		right = _left + _width;
		top = _top;
		bottom = _top + height;
	}

	int getLeft()   { return left; }
	int getRight()   { return right; }
	int getWidth()   { return width; }
	int getHeight()   { return height; }
	int getTop()   { return top; }
	int getBottom()   { return bottom; }

	void set2(int l, int t, int width, int height)
	{
		left = l;
		right = l + width;
		this->width = width; 
		this->height = height;
		top = t;
		bottom = t + height;
	}

	void set(int l, int r, int t, int b)
	{
		left = l;
		right = r;
		top = t;
		bottom = b;
		width = r - l;
		height = b - t;
	}

	void crop(int l, int r, int t, int b)
	{
		left = max(l, left);  right = max(l, right);
		left = min(r, left);  right = min(r, right);
		top = max(t, top);    bottom = max(t, bottom);
		top = min(b, top);    bottom = min(b, bottom);
		width = right - left;
		height = bottom - top;
	}

	bool operator==(const SimpleRect &r)
	{
		if (r.left != left || r.right != right || r.top != top || r.bottom != bottom)
			return false;
		return true;
	}

	bool containPoint(SimplePoint<int> p)
	{
		if (p.x>left && p.x<right && p.y>top && p.y<bottom)
			return true;
		else
			return false;
	}

private:
	int left;
	int right;
	int top;
	int bottom;
	int width;
	int height;

};
