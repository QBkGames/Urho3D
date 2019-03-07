//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Math/Vector2.h"

namespace Urho3D
{

class Rect;

/// %Circle in two-dimensional space.
class URHO3D_API Circle
{
public:
    /// Construct undefined.
    Circle() noexcept :
        center_(Vector2::ZERO),
        radius_(-M_INFINITY)
    {
    }

    /// Copy-construct from another Circle.
    Circle(const Circle& Circle) noexcept = default;

    /// Construct from center and radius.
    Circle(const Vector2& center, float radius) noexcept :
        center_(center),
        radius_(radius)
    {
    }

    /// Construct from an array of vertices.
    Circle(const Vector2* vertices, unsigned count) noexcept
    {
        Define(vertices, count);
    }

    /// Construct from a rectangle.
    explicit Circle(const Rect& rect) noexcept
    {
        Define(rect);
    }

    /// Assign from another Circle.
    Circle& operator =(const Circle& rhs) noexcept = default;

    /// Test for equality with another Circle.
    bool operator ==(const Circle& rhs) const { return center_ == rhs.center_ && radius_ == rhs.radius_; }

    /// Test for inequality with another Circle.
    bool operator !=(const Circle& rhs) const { return center_ != rhs.center_ || radius_ != rhs.radius_; }

    /// Define from another Circle.
    void Define(const Circle& circle)
    {
        Define(circle.center_, circle.radius_);
    }

    /// Define from center and radius.
    void Define(const Vector2& center, float radius)
    {
        center_ = center;
        radius_ = radius;
    }

    /// Define from an array of vertices.
    void Define(const Vector2* vertices, unsigned count);
    /// Define from a rectangle.
    void Define(const Rect& rect);

    /// Merge a point.
    void Merge(const Vector2& point)
    {
        if (radius_ < 0.0f)
        {
            center_ = point;
            radius_ = 0.0f;
            return;
        }

        Vector2 offset = point - center_;
        float dist = offset.Length();

        if (dist > radius_)
        {
            float half = (dist - radius_) * 0.5f;
            radius_ += half;
            center_ += (half / dist) * offset;
        }
    }

    /// Merge an array of vertices.
    void Merge(const Vector2* vertices, unsigned count);
    /// Merge a rectangle.
    void Merge(const Rect& rect);
    /// Merge a Circle.
    void Merge(const Circle& Circle);

    /// Clear to undefined state.
    void Clear()
    {
        center_ = Vector2::ZERO;
        radius_ = -M_INFINITY;
    }

    /// Return true if this Circle is defined via a previous call to Define() or Merge().
    bool Defined() const
    {
        return radius_ >= 0.0f;
    }

    /// Test if a point is inside.
    Intersection IsInside(const Vector2& point) const
    {
        float distSquared = (point - center_).LengthSquared();
        if (distSquared < radius_ * radius_)
            return INSIDE;
        else
            return OUTSIDE;
    }

    /// Test if another Circle is inside, outside or intersects.
    Intersection IsInside(const Circle& circle) const
    {
        float dist = (circle.center_ - center_).Length();
        if (dist >= circle.radius_ + radius_)
            return OUTSIDE;
        else if (dist + circle.radius_ < radius_)
            return INSIDE;
        else
            return INTERSECTS;
    }

    /// Test if another Circle is (partially) inside or outside.
    Intersection IsInsideFast(const Circle& circle) const
    {
        float distSquared = (circle.center_ - center_).LengthSquared();
        float combined = circle.radius_ + radius_;

        if (distSquared >= combined * combined)
            return OUTSIDE;
        else
            return INSIDE;
    }

    /// Test if a rectangle is inside, outside or intersects.
    Intersection IsInside(const Rect& rect) const;
    /// Test if a rectangle is (partially) inside or outside.
    Intersection IsInsideFast(const Rect& rect) const;

    /// Return distance of a point to the surface, or 0 if inside.
    float Distance(const Vector2& point) const { return Max((point - center_).Length() - radius_, 0.0f); }

    /// Circle center.
    Vector2 center_;
    /// Circle radius.
    float radius_{};

	/// Circle at origin with radius 1
	static const Circle UNIT;
	/// Circle at origin with radius 0
	static const Circle ZERO;
};

}
