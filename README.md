# Vulkan Ray Tracing Renderer: The Next-Level Graphics Engine

![UI Sample](images/UI_sample.png)

This is an advanced graphics rendering project built on top of the Walnut App Template. It utilizes the Vulkan API to produce high-quality ray-traced graphics. Unlike the basic Walnut example, this project keeps Walnut as an external submodule, ideal for building scalable and complex applications.

## Status: In Development

## Role: Solo Developer

---

## Getting Started

1. Clone this repository.
2. Customize the `premake5.lua` and `RayTracing/RayTracingRenderer/premake5.lua` files as needed.
3. Run `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files.
4. Your main app is located in the `RayTracing/RayTracingRenderer/src/WalnutApp.cpp` directory. Start modifying this project to suit your needs.

---

## Features

### Core Features

- Ray and Sphere Calculations
- Sphere Rendering
- Ray Casting
- Lighting and Shading
- Interactive 3D Camera System
- Multi-Object Rendering
- Ray Tracing Pipeline
- Physically Based Materials
- Path Tracing
- Multithreading

![CPU Rendering Sample](images/CPU_Rendering_Sample.png)

### Additional Features

- Performance Optimization
- Emissive Materials
- Fast Random Generation

### UI Options

- Enable/Disable Accumulation for Noise
- Change Materials and Transforms of Objects

---

## Technologies Used

- Vulkan
- C++
- Walnut as an external submodule

---

## Additional Notes

- **Development Path**: This project originated as a CPU-based renderer and is now transitioning to leverage GPU capabilities.
  
- **Future Considerations**: Techniques like NVIDIA DLSs (Deep Learning Super Sampling) or AI Sampling may be integrated in the future.

- **Compatibility**: This project is based on the Walnut App Template and can serve as a foundational base for future game engines.

---

## UPCOMING

![GPU Rendering Upcoming](images/GPU_Rendering_Upcoming.png)

Transition to full GPU-based rendering is currently underway to unlock next-level performance and quality.
