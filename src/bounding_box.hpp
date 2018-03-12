#ifndef MMAP_DEMO_BOUNDING_BOX_HPP
#define MMAP_DEMO_BOUNDING_BOX_HPP

#undef near
#undef far

template<typename T>
class bounding_box {
    T _left {};
    T _top {};
    T _right {};
    T _bottom {};
public:
    bounding_box() = default;
    bounding_box(T left, T bottom, T right, T top) noexcept
    : _left{left}
    , _top{top}
    , _right{right}
    , _bottom{bottom} {

    }

    bool intersect(const bounding_box& other) const noexcept {
        return !(other._right < _left ||
                 other._left > _right ||
                 other._top < _bottom ||
                 other._bottom > _top);
    }

    T left() const noexcept {
        return _left;
    }

    T right() const noexcept {
        return _right;
    }

    T bottom() const noexcept {
        return _bottom;
    }

    T top() const noexcept {
        return _top;
    }

    T width() const noexcept {
        return _right - _left;
    }

    T height() const noexcept {
        return _top - _bottom;
    }
};

template<typename T>
class bounding_cube {
    T left {};
    T top {};
    T near{};
    T right{};
    T bottom{};
    T far{};
public:
    bounding_cube() = default;
    bounding_cube(T left, T top, T near, T right, T bottom, T far) noexcept
    : left{left}
    , top{top}
    , near{near}
    , right{right}
    , bottom{bottom}
    , far{far} {

    }

    bool intersect(const bounding_cube& other) const noexcept {
        return !(other.right < left ||
                 other.left > right ||
                 other.top < bottom ||
                 other.bottom > top ||
                 other.far < near   ||
                 other.near > far);
    }
};

#endif
