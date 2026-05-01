#include "basic.h"
#include <Engine/window.h>

int main() {
    Window::initialize(1280, 720, "Escape from the Wizard's Dungeon");
    Window::addGame(std::make_unique<BasicGame>());

    // Create game
    std::unique_ptr<Game> basic_game = std::make_unique<BasicGame>();

    while (Window::isActive()) {
        Window::update();
    }
    Window::shutDown();

    return 0;
}
