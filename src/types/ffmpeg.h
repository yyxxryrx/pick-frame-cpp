//
// Created by yyxxryrx on 2026/9/26.
//

#ifndef PICK_FRAME_FFMPEG_H
#define PICK_FRAME_FFMPEG_H
#include <memory>

template<typename T, void (*FreeFn)(T **)>
struct FFDestructor1 {
    void operator()(T *p) const noexcept {
        if (p) {
            T *tmp = p;
            FreeFn(&tmp);
        }
    }
};

template<typename T, void (*FreeFn)(T *)>
struct FFDestructor2 {
    void operator()(T *p) const noexcept {
        if (p) FreeFn(p);
    }
};

template<typename T, void (*FreeFn)(T **)>
using FFPtr1 = std::unique_ptr<T, FFDestructor1<T, FreeFn> >;

template<typename T, void (*FreeFn)(T *)>
using FFPtr2 = std::unique_ptr<T, FFDestructor2<T, FreeFn> >;

#endif //PICK_FRAME_FFMPEG_H
