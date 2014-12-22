This is the reference code that I used as the basis of comparison when I
first wrote DirectX 9 Managed code. These demos should be comparable to
the corresponding demos in the
[ManagedDX9Tutorial](https://github.com/AceRoqs/directx_tutorials/tree/master/ManagedDX9Tutorial)
series, though I never published the C++ version previously.

This code was compiled with Visual C++ 2013, though porting it to
previous versions of Visual C++ should be trivial as the code was
originally developed on Visual C++ 2003 and no C++11 features are used.

This code was derived from my DirectX 8 tutorial code written over a
decade ago, so my current coding practices differ a bit from what is
shown here. The code is in a tutorial style, so shortcuts \(such as the
lack of precompiled headers\) are taken for clarity.

* Sample 1: Initializes and shuts down DirectX, and runs a basic render loop.
* Sample 2: Draws triangles with a vertex buffer.
* Sample 3: Draws triangles with an index buffer.
* Sample 4: Adds a texture to the drawn triangles.
* Sample 5: Spins a cube of textured triangles.
* Sample 6: Uses assembly shaders instead of a fixed function pipeline.

The [DirectX SDK \(June 2010\)](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
is the latest DirectX SDK as of 2015, and is required to build these examples.
[KB 2728613](https://support.microsoft.com/kb/2728613/en-us) will likely be
required on modern systems.

Toby Jones \([www.turbohex.com](http://www.turbohex.com), [ace.roqs.net](http://ace.roqs.net)\)

