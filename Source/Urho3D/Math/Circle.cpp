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

#include "../Precompiled.h"

#include "../Math/Circle.h"
#include "../Math/Rect.h"

#include "../DebugNew.h"

namespace Urho3D
{

const Circle Circle::UNIT(Vector2(), 1.0f);
const Circle Circle::ZERO(Vector2(), 0.0f);

void Circle::Define(const Vector2* vertices, unsigned count)
{
    if (!count)
        return;

    Clear();
    Merge(vertices, count);
}

void Circle::Define(const Rect& rect)
{
    const Vector2& min = rect.min_;
    const Vector2& max = rect.max_;

    Clear();
    Merge(min);
    Merge(Vector2(max.x_, min.y_));
    Merge(Vector2(min.x_, max.y_));
    Merge(max);
}

void Circle::Merge(const Vector2* vertices, unsigned count)
{
    while (count--)
        Merge(*vertices++);
}

void Circle::Merge(const Rect& rect)
{
    const Vector2& min = rect.min_;
    const Vector2& max = rect.max_;

    Merge(min);
    Merge(Vector2(max.x_, min.y_));
    Merge(Vector2(min.x_, max.y_));
    Merge(max);
}

void Circle::Merge(const Circle& Circle)
{
    if (radius_ < 0.0f)
    {
        center_ = Circle.center_;
        radius_ = Circle.radius_;
        return;
    }

    Vector2 offset = Circle.center_ - center_;
    float dist = offset.Length();

    // If Circle fits inside, do nothing
    if (dist + Circle.radius_ < radius_)
        return;

    // If we fit inside the other Circle, become it
    if (dist + radius_ < Circle.radius_)
    {
        center_ = Circle.center_;
        radius_ = Circle.radius_;
    }
    else
    {
        Vector2 NormalizedOffset = offset / dist;

        Vector2 min = center_ - radius_ * NormalizedOffset;
        Vector2 max = Circle.center_ + Circle.radius_ * NormalizedOffset;
        center_ = (min + max) * 0.5f;
        radius_ = (max - center_).Length();
    }
}

Intersection Circle::IsInside(const Rect& rect) const
{
    float radiusSquared = radius_ * radius_;
    float distSquared = 0;
    float temp;
    Vector2 min = rect.min_;
    Vector2 max = rect.max_;

    if (center_.x_ < min.x_)
    {
        temp = center_.x_ - min.x_;
        distSquared += temp * temp;
    }
    else if (center_.x_ > max.x_)
    {
        temp = center_.x_ - max.x_;
        distSquared += temp * temp;
    }
    if (center_.y_ < min.y_)
    {
        temp = center_.y_ - min.y_;
        distSquared += temp * temp;
    }
    else if (center_.y_ > max.y_)
    {
        temp = center_.y_ - max.y_;
        distSquared += temp * temp;
    }

    if (distSquared >= radiusSquared)
        return OUTSIDE;

    min -= center_;
    max -= center_;

    Vector2 tempVec = min; // - -
    if (tempVec.LengthSquared() >= radiusSquared)
        return INTERSECTS;
    tempVec.x_ = max.x_; // + -
    if (tempVec.LengthSquared() >= radiusSquared)
        return INTERSECTS;
    tempVec.y_ = max.y_; // + +
    if (tempVec.LengthSquared() >= radiusSquared)
        return INTERSECTS;
    tempVec.x_ = min.x_; // - +
    if (tempVec.LengthSquared() >= radiusSquared)
        return INTERSECTS;

    return INSIDE;
}

Intersection Circle::IsInsideFast(const Rect& rect) const
{
    float radiusSquared = radius_ * radius_;
    float distSquared = 0;
    float temp;
    Vector2 min = rect.min_;
    Vector2 max = rect.max_;

    if (center_.x_ < min.x_)
    {
        temp = center_.x_ - min.x_;
        distSquared += temp * temp;
    }
    else if (center_.x_ > max.x_)
    {
        temp = center_.x_ - max.x_;
        distSquared += temp * temp;
    }
    if (center_.y_ < min.y_)
    {
        temp = center_.y_ - min.y_;
        distSquared += temp * temp;
    }
    else if (center_.y_ > max.y_)
    {
        temp = center_.y_ - max.y_;
        distSquared += temp * temp;
    }

    if (distSquared >= radiusSquared)
        return OUTSIDE;
    else
        return INSIDE;
}

}
