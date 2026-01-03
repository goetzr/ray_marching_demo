Absolutely — and this is one of those fun “old‑school graphics programming” moments that fits your systems‑programming mindset beautifully.
Let’s break down what “SDL3 for the window and pixel buffer only” really means, because it does imply that you’re not using SDL’s 2D rendering API at all.

🧱 The Core Idea
You’re building a software renderer — meaning your C++ code computes every pixel of the ray‑marched image on the CPU. SDL3’s job is simply to:
create a window
give you a chunk of memory representing the screen
let you copy your computed pixels into that memory
present it to the user
That’s it. No GPU shaders, no OpenGL, no SDL_RenderCopy, no SDL textures unless you want them as a blit target.
This is exactly how classic engines (Doom, Quake 1, early ray tracers) worked.

🖼️ So what does the rendering loop look like?
1. You allocate a pixel buffer (e.g., uint32_t* pixels)
This is just a linear array of RGBA or ARGB values.
2. Each frame, your ray marcher writes into that buffer
For each pixel:
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        pixels[y * width + x] = raymarch_pixel(x, y);
    }
}

raymarch_pixel() does all the SDF math, lighting, shadows, etc.
3. SDL3 uploads that buffer to the window
There are two common ways:

🟦 Method A: SDL_Surface + SDL_BlitSurface (simple)
You create an SDL_Surface that uses your pixel buffer as its backing memory:
SDL_Surface* surface = SDL_CreateSurfaceFrom(
    width, height,
    SDL_PIXELFORMAT_ARGB8888,
    pixels,
    width * sizeof(uint32_t)
);

Then each frame:
SDL_BlitSurface(surface, nullptr, window_surface, nullptr);
SDL_UpdateWindowSurface(window);

This is the simplest approach.

🟩 Method B: SDL_Texture + SDL_UpdateTexture (faster)
You create a streaming texture:
SDL_Texture* tex = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    width, height
);

Then each frame:
SDL_UpdateTexture(tex, nullptr, pixels, width * sizeof(uint32_t));
SDL_RenderCopy(renderer, tex, nullptr, nullptr);
SDL_RenderPresent(renderer);

Even though this uses the SDL renderer, you’re not using SDL to draw anything — you’re just uploading your CPU‑generated pixels into a texture.
This is still “CPU rendering” because the GPU is only doing a final blit.

🟥 So to answer your question directly
Yes — you are not using SDL’s rendering primitives.
No:
SDL_RenderDrawLine
SDL_RenderFillRect
SDL_RenderGeometry
SDL_RenderCopy (except for the final blit)
Your ray marcher computes the entire image in software.
SDL is just the display layer.

🧠 Why this approach is perfect for a 12–15 hour ray marcher
You avoid GPU shader boilerplate
You avoid debugging GLSL/WGSL
You can step through your ray marcher in a debugger
You can print intermediate values
You can write everything in pure C++
You get deterministic behavior across platforms
It’s the fastest way to get a working ray marcher without fighting GPU APIs.

🎯 If you want, I can sketch the minimal C++ skeleton
I can show you:
the SDL3 initialization
the pixel buffer setup
the main loop
a stub raymarch_pixel()
the final blit
Just say the word and I’ll lay out the whole structure cleanly.
