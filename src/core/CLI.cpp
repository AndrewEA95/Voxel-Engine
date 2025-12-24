#include "CLI.h"
#include <iostream>
#include <string>

namespace Core {

bool CLI::handleCommandLine(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            std::cout << "Voxel Engine — Runtime Options\n";
            std::cout << "----------------------------------------\n";
            std::cout << "Environment Variables:\n";
            std::cout << "  VOXEL_LOG_LEVEL=LEVEL   Set minimum log level\n";
            std::cout << "  VOXEL_LOG_TRACE=1       Enable TRACE logs\n";
            std::cout << "  VOXEL_LOG_DEBUG=1       Enable DEBUG logs\n";
            std::cout << "\nExamples:\n";
            std::cout << "  VOXEL_LOG_LEVEL=INFO ./voxel_engine\n";
            std::cout << "  VOXEL_LOG_TRACE=1 ./voxel_engine\n";
            std::cout << "  VOXEL_LOG_DEBUG=1 ./voxel_engine\n";
            std::cout << "\nFlags:\n";
            std::cout << "  --help, -h              Show this help message\n";
            std::cout << " --VERSION, -v.            Shoes the current version of the game\n";
            return true;
        }

        if (arg == "--VSERION" || arg == "-v") {
            std::cout << "Voxel Engine v0.1.0\n";
            return true;
        }
    }

    return false;
}

}