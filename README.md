fake[Owl](https://github.com/owl-project/owl)
===========

A CPU implementation of OWL. Very experimental :-)

ATM only works on macOS, as my main dev machine is a Mac. I had it running on Linux, too, but 2-3 changes would be required. (At the very least, you'd have to replace `.dylib` with `.so` in [cmake/configure_fake_owl.cmake](/cmake/configure_fake_owl.cmake), but there's probably 1-2 more changes necessary that I just don't remember rn.)

For the interactive samples you also need a checkout of [cuteeOwl](https://github.com/owl-project/cuteeOWL). You'd have to place that in the top level dir of fakeOwl. There's a [patch](0001-Patch-to-cuteeOwl-for-compat-with-fakeOwl.patch) I've placed there that you'd have to apply to cuteeOwl. Pretty hacky, I know.

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

fakeOwl is based off of stable OWL releases; the basis for new fakeOwl versions form the public header files under [include/owl/](https://github.com/owl-project/owl/tree/master/owl/include/owl). The current fakeOwl version is based on the commit `ca8f7b7e72db600fa7f993cf6e4f471e5895983e` (OWL master as of Sat Aug 21 13:56:13 2021 -0600).

License
-------

MIT if not noted otherwise. I'm including lot's of code that was directly copied from OWL, the license there is Apache 2.

