#pragma once

namespace Core
{
    class CLI
    {
        public:
            // Returns true if program should exit after handling CLI
            static bool handleCommandLine(int argc, char** argv);
    };
}