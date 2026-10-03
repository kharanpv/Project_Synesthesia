# Troubleshooting

## 1. Blank Window on Linux (Wayland/X11)

**Symptom:** App prints "Engine fully booted. Entering main loop." but no window appears.

**Cause:** SDL2 was built without a usable video driver (Wayland or X11). The required EGL dev libraries (`libegl-dev`) are missing, so SDL2 compiles only the DUMMY driver which cannot create visible windows.

**Fix:** Install the required development libraries before building:
```bash
sudo apt install libegl-dev libgles-dev libwayland-dev libxkbcommon-dev wayland-protocols libx11-dev
```
Then rebuild:
```bash
rm -rf build && cmake -S . -B build -G Ninja && cmake --build build
```

**Verify:** In the CMake configure output, confirm `SDL_WAYLAND` or `SDL_X11` shows `ON`:
```
SDL_WAYLAND  (Wanted: ON): ON
SDL_X11      (Wanted: ON): ON
```
