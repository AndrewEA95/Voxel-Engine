#include "core/CLI.h"
#include "core/Engine.h"

int main(int argc, char** argv)
{
    if (Core::CLI::handleCommandLine(argc, argv))
        return 0;

    Core::Engine engine;
    engine.run();
    return 0;
}