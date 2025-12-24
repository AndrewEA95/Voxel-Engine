#pragma once

namespace Core 
{

    class Time 
    {
        public:
            static void update();
            static float deltaTime();

        private:
            static float s_deltaTime;
            static double s_lastTime;
    };

}