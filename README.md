# OksEngine

**A personal, modern game engine built from scratch with C++20, Vulkan, ECS and Lua.**

OksEngine is an experimental real-time engine designed to explore cutting-edge graphics, data-oriented architecture and flexible scripting – all while keeping full control over the core technology. It is developed out of passion, with the goal of creating a strong foundation for future game projects.

> ⚠️ The engine is currently in active, early development. APIs, features and workflows may change frequently. No stable releases yet.

---

## ✨ Features

- **Modern Graphics**  
  Vulkan-based renderer with support for glTF/GLB models and skeletal animations.

- **Data-Oriented ECS**  
  Custom entity-component-system architecture (`.ecs` files, Lua-based system definitions) with automatic code generation.

- **Lua Scripting**  
  Full game logic can be written in Lua; attach scripts directly to entities (e.g. `Character.lua`, `Camera.lua`).

- **Runtime Configuration**  
  All engine settings (window, input bindings, rendering, etc.) are driven by a single `config.lua` file.

- **Integrated Physics**  
  Rigid body simulation, ragdolls and collision detection.

- **In-Game Debugging**  
  Immediate-mode GUI (ImGui) for runtime inspection and tweaking.

- **Scene Save/Load**  
  Serialization and deserialization of entire scenes.

- **Developer Tooling**  
  A built-in ECS code generator and various utilities to speed up iteration.

---

## 🧱 Tech Stack

| Area           | Technology                        |
| -------------- | --------------------------------- |
| Core Language  | C++20                             |
| Build System   | CMake                             |
| Package Manager| vcpkg                             |
| Graphics API   | Vulkan                            |
| Scripting      | Lua                               |
| Debug UI       | ImGui                             |
| Code Generation| Python (ECS generator)            |
| Assets         | glTF 2.0 (`.glb`)                 |

---

## 🚀 Getting Started

### Prerequisites

- **CMake** 3.21+
- **C++20** capable compiler (MSVC 2022, Clang 15+, GCC 12+)
- **Vulkan SDK** ([LunarG](https://vulkan.lunarg.com/))
- **vcpkg** ([installation guide](https://github.com/microsoft/vcpkg#quick-start-windows))
- **Python 3** (for code generation tools)
- **Git**

### Build Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/olegoks/OksEngine.git
   cd OksEngine

## 📁 Project Structure

OksEngine/
├── Sources/
│ ├── Engine/ # Engine core (C++)
│ │ ├── Sources/ # ECS systems (*.ecs files)
│ │ ├── Libraries/ # Third‑party dependencies (via vcpkg)
│ │ └── Tests/ # Unit tests
│ ├── Scripts/ # Lua gameplay scripts
│ └── main.cpp # Entry point
├── Scripts/ # Shell scripts (build automation)
├── Tools/
│ ├── ECSGenerator2/ # Custom ECS code generator
│ └── Control/ # Development utilities
├── Documents/ # Detailed specs (e.g. OBJ format)
├── KnowledgeDB/ # Developer’s personal Obsidian notes
├── config.lua # Engine configuration
└── CMakeLists.txt