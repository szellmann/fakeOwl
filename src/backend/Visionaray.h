
#pragma once

#include <thread>

#ifdef __CUDACC__
#undef __CUDACC__
#endif

#include <visionaray/detail/parallel_for.h>
#include <visionaray/detail/thread_pool.h>

namespace fake
{
    namespace backend
    {
        struct Visionaray
        {
            template <typename Func>
            void parallel_for(int32_t minX, int32_t maxX, int32_t strideX,
                              const Func &func)
            {
                visionaray::parallel_for(
                    threadPool,
                    visionaray::tiled_range1d<int32_t>(minX,maxX,strideX),
                    [&](visionaray::range1d<int32_t> r) {
                        for (int i = r.begin(); i != r.end(); ++i)
                        {
                            func(i);
                        }
                    });
            }

            template <typename Func>
            void parallel_for(int32_t minX, int32_t maxX, int32_t strideX,
                              int32_t minY, int32_t maxY, int32_t strideY,
                              const Func &func)
            {
                visionaray::parallel_for(
                    threadPool,
                    visionaray::tiled_range2d<int32_t>(minX,maxX,strideX,
                                                       minY,maxY,strideY),
                    [&](visionaray::range2d<int32_t> r)
                    {
                        for (int y = r.cols().begin(); y < r.cols().end(); ++y)
                        {
                            for (int x = r.rows().begin(); x < r.rows().end(); ++x)
                            {
                                func(x,y);
                            }
                        }
                    });
            }

            visionaray::thread_pool threadPool{std::thread::hardware_concurrency()};
        };

    } // backend
} // visionaray

