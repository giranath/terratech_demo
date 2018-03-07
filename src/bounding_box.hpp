#ifndef MMAP_DEMO_BOUNDING_BOX_HPP
#define MMAP_DEMO_BOUNDING_BOX_HPP

template<typename T>
class bounding_box {
    T left {};
    T top {};
    T right {};
    T bottom {};
public:
    bounding_box() = default;
    bounding_box(T left, T bottom, T right, T top) noexcept
    : left{left}
    , top{top}
    , right{right}
    , bottom{bottom} {

    }

    bool intersect(const bounding_box& other) const noexcept {
        return !(other.right < left ||
                 other.left > right ||
                 other.top < bottom ||
                 other.bottom > top);
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
