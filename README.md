fake[Owl](https://github.com/owl-project/owl)
===========

A CPU implementation of OWL. Very experimental :-)

ATM only works on macOS, as my main dev machine is a Mac. I had it running on Linux, too, but 2-3 patches would need to be applied. (At the very least, you'd have to replace `.dylib` with `.so` in `cmake/configure_fake_owl.cmake`, but there's probably 1-2 more changes necessary that I just don't remember rn.)

For the interactive samples you also need a checkout of [cuteeOwl](https://github.com/owl-project/cuteeOWL). You'd have to place that in the top level dir of fakeOwl. There's a patch I've placed there that you'd have to apply to cuteeOwl. Pretty hacky, I know.

The CPU intersections are computed using the [Visionaray](https://github.com/szellmann/visionaray) submodule, but honestly intersections are less than 1% of this library. We could probably use Embree here w/o much effort, but I'm using Visionaray b/c it comes with a nice texture abstraction layer, and I'm also more familiar with Visionaray than with Embree, so I never bothered to replace it so far.

Porting existing OWL code
-------------------------

I'm currently only supporting a subset of OWL and adding features only as needed for my own projects. Also note OWL CPU programs are pretty slow due to certain conceptual limitations (mostly function call overhead and costly access to TLS). So it's mostly useful for debugging etc.

The following changes are required at the very least:

Replace `owlXXXBufferCreate()` with `owlBufferCreateEXT()` from `owl_ext.h` (uses malloc/free instead of cudaMalloc et al.).

In your "device code":

At the top of the files, add:
```
FAKE_OWL_MODULE_DECL // diff-1
```

And for launch params declarations, add init parantheses:
```
extern "C" __constant__ LaunchParams optixLaunchParams {}; // diff-2 (initialization)
```

That's mostly it. _Some_ platform-specific CUDA stuff works, but most (obviously) doesn't. Have a look in the `fake/cuda.h` and `fake/optix.h` files. The OptiX functions should _eventually_ be ported in their entirety but the CUDA stuff is only there for convenience. BTW, there is a `clock64()` implementation in `fake/cuda.h` that you should use instead of `clock()` on x86, as the latter will perform syscalls and is awfully slow (some of the owl samples use `clock()`).

Licence
-------

MIT if not noted otherwise. I'm including lot's of code that was directly copied from OWL, the license there is Apache 2.

