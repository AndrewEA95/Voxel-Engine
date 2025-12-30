#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <thread>
#include <atomic>

#include "../voxel/Chunk.h"
#include "../scene/Scene.h"

struct ChunkCoordHash
{
    size_t operator()(const ChunkCoord& c) const
    {
        return (std::hash<int>()(c.x) * 73856093) ^
               (std::hash<int>()(c.y) * 19349663) ^
               (std::hash<int>()(c.z) * 83492791);
    }
};

struct ChunkBuildResult
{
    ChunkCoord coord;
    Chunk chunk;
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    ChunkBuildResult(const ChunkCoord& c,
                     Chunk&& ch,
                     std::vector<float>&& v,
                     std::vector<uint32_t>&& i)
        : coord(c), chunk(std::move(ch)),
          vertices(std::move(v)), indices(std::move(i))
    {}
};

class ChunkManager
{
public:
    ChunkManager(Scene& scene);
    ~ChunkManager();

    void update(const glm::vec3& cameraPos);

    Voxel getVoxelAt(int wx, int wy, int wz) const;
    const Chunk* getChunk(const ChunkCoord& coord) const;

private:
    Scene& m_scene;

    std::unordered_map<ChunkCoord, Chunk, ChunkCoordHash> m_chunks;

    int m_loadRadius; // chunks in each direction
    int m_verticalRadius;

    TerrainGenerator m_terrain; 

    void loadChunk(const ChunkCoord& coord);
    void unloadFarChunks(const ChunkCoord& center);
    void unloadChunk(const ChunkCoord& coord);

    ChunkCoord worldToChunk(const glm::vec3& pos);

    bool m_startupBurst = true;

    std::vector<std::thread> m_workers;
    bool m_shutdown = false;

    std::mutex m_jobMutex;
    std::condition_variable m_jobCv;
    std::queue<ChunkCoord> m_jobs;

    std::unordered_set<ChunkCoord> m_chunksInProgress;

    std::mutex m_resultMutex;
    std::queue<ChunkBuildResult> m_results;

    ChunkCoord m_lastCenter{};
    bool m_hasLastCenter = false;

    void workerThread(); // same name, new behavior

    void pumpResults();
};