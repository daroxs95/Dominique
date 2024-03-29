#if !defined(DENGINE_CORE)
#define DENGINE_CORE

#include <functional>
#include "ecs/ecs.hpp"

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;

namespace de {
enum class RendererType {
  OpenGl,
  // DirectX11
};

struct DEConfig {
  float clearColor[4];
  const char *title;
  int width;
  int height;
  int flags;
};

struct DE {
  RendererType rendererType = RendererType::OpenGl;
  SDL_Window *windowHandler = nullptr;
  SDL_GLContext glContext = nullptr;
  DEConfig config = {0.03f, 0.69f, 0.48f, 0.0f, "DEngine App", 800, 600};
};

namespace core {
// OpenGl related
int Init(DE &engine);
int Clean(DE &engine);
void Render(DE &engine);
void Run(DE &engine, de::ecs::Scene &scene);
};  // namespace core

}  // namespace de

#endif  // DENGINE_CORE
