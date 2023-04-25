fake[Owl](https://github.com/owl-project/owl)
===========

A CPU implementation of OWL. Very experimental :-)

The CPU intersections are computed using the [Visionaray](https://github.com/szellmann/visionaray) submodule, but honestly intersections are less than 1% of this library. We could probably use Embree here w/o much effort, but I'm using Visionaray b/c it comes with a nice texture abstraction layer, and I'm also more familiar with Visionaray than with Embree, so I never bothered to replace it so far.

Porting existing OWL code
-------------------------

I'm currently only supporting a subset of OWL and adding features only as needed for my own projects. Also note that OWL CPU programs are pretty slow due to certain conceptual limitations (mostly function call overhead and costly access to TLS). So it's mostly useful for debugging etc.

The following changes are necessary to an existing code base at the very least:

Replace `owlXXXBufferCreate()` with `owlBufferCreateEXT()` from [owl_ext.h](/include/owl/owl_ext.h) (uses malloc/free instead of cudaMalloc et al.).

The device code _should_ mostly just work. See the comments on CUDA below, and it _might_ be necessary to shuffle some headers around in case that declarations are slightly different than in _real_ OptiX.
In your "device code":

CMake is a bit different of course. You have to link with libfakeOwl.{a|dylib} and you want to check out the macro `fake_owl_compile_and_embed` in [cmake/configure_fake_owl.cmake](/cmake/configure_fake_owl.cmake). With my cmake version (3.19.1) that macro would fail if being passed `.cu` files, therefore I'm renaming the files where the optix device programs would usually go to `.cpp` (another option would be to use symlinks).

That's mostly it. _Some_ platform-specific CUDA stuff works, but most (obviously) doesn't. Have a look in the [fake/cuda.h](/include/fake/cuda.h) and [fake/optix.h](/include/fake/optix.h) files. The OptiX functions should _eventually_ be ported in their entirety but the CUDA stuff is only there for convenience. BTW, there is a `clock64()` implementation in `fake/cuda.h` that you should use instead of `clock()` on x86, as the latter will perform syscalls and is awfully slow (some of the owl samples use `clock()`).

OWL compatibility
-----------------

fakeOwl is based off of stable OWL releases; the basis for new fakeOwl versions form the public header files under [include/owl/](https://github.com/owl-project/owl/tree/master/owl/include/owl). The current fakeOwl version is based on the commit `8c4d5f1ea0cc2a1974ed8111381c1d26e6417af8` (OWL master as of Tue Apr 18 13:37:02 2023 -0600). See [fake/verision.h](/include/fake/version.h) for details.

License
-------

MIT if not noted otherwise. I'm including lot's of code that was directly copied from OWL, the license there is Apache 2.

