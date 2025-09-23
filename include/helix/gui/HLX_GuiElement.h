#pragma once

namespace HLX {
class GuiElement {
  public:
    GuiElement() {};
    virtual ~GuiElement() = default;
    virtual void render() = 0;
};
}; // namespace HLX
