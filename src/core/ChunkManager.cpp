#include "ChunkManager.h"
#include "../voxel/Chunk.h"
#include "../voxel/ChunkMesher.h"
#include "../scene/Scene.h"
#include "../render/RenderObject.h"

#include "iostream"

#include <chrono>
#include <cmath>
#include <mutex>
#include <thread>
#include <vector>

ChunkManager::ChunkManager(Scene& scene)
    : m_scene(scene)
{
    m_loadRadius = 4;        // tune later
    m_verticalRadius = 2;    // tune later

    m_shutdown = false;
    m_hasLastCenter = false;

    // Start 2 worker threads (your MacBook Air can handle this)
    const int workerCount = 2;
    m_workers.reserve(workerCount);

    for (int i = 0; i < workerCount; ++i)
    {
        m_workers.emplace_back(&ChunkManager::workerThread, this);
    }
}

ChunkManager::~ChunkManager()
{
    {
        std::lock_guard<std::mutex> lock(m_jobMutex);
        m_shutdown = true;
        std::queue<ChunkCoord> empty;
        std::swap(m_jobs, empty); // clear all pending jobs
    }

    m_jobCv.notify_all();

    for (auto& t : m_workers)
    {
        if (t.joinable())
            t.join();
    }
}

ChunkCoord ChunkManager::worldToChunk(const glm::vec3& pos)
{
        return {
        (int)std::floor(pos.x / Chunk::SIZE),
        (int)std::floor(pos.y / Chunk::SIZE),
        (int)std::floor(pos.z / Chunk::SIZE)
    };
}

void ChunkManager::update(const glm::vec3& cameraPos)
{
    // 1. Always pump finished chunks (GPU upload)
    pumpResults();

    // 2. Compute current center chunk
    ChunkCoord center = worldToChunk(cameraPos);

    // 3. Build list of all chunk coords in radius
    std::vector<ChunkCoord> coords;
    coords.reserve((m_loadRadius * 2 + 1) *
                   (m_loadRadius * 2 + 1) *
                   (m_verticalRadius * 2 + 1));

    for (int x = -m_loadRadius; x <= m_loadRadius; x++)
    for (int y = -m_verticalRadius; y <= m_verticalRadius; y++)
    for (int z = -m_loadRadius; z <= m_loadRadius; z++)
    {
        coords.push_back({ center.x + x, center.y + y, center.z + z });
    }

    // 4. Sort by distance to center (closest chunks first)
    std::sort(coords.begin(), coords.end(),
        [&](const ChunkCoord& a, const ChunkCoord& b)
        {
            int da = (a.x - center.x)*(a.x - center.x)
                   + (a.y - center.y)*(a.y - center.y)
                   + (a.z - center.z)*(a.z - center.z);

            int db = (b.x - center.x)*(b.x - center.x)
                   + (b.y - center.y)*(b.y - center.y)
                   + (b.z - center.z)*(b.z - center.z);

            return da < db;
        });

    // 5. Schedule chunk jobs in sorted order
    int jobsScheduled = 0;
     int maxJobsPerFrame = 256; // crank this up now

    if (m_startupBurst)
    {
        maxJobsPerFrame = 5000; // schedule EVERYTHING immediately
        m_startupBurst = false;
    }

    for (const ChunkCoord& coord : coords)
    {
        if (m_chunks.find(coord) == m_chunks.end() &&
            m_chunksInProgress.find(coord) == m_chunksInProgress.end())
        {
            {
                std::lock_guard<std::mutex> lock(m_jobMutex);
                m_jobs.push(coord);
            }
            m_chunksInProgress.insert(coord);
            m_jobCv.notify_one();

            jobsScheduled++;
            if (jobsScheduled >= maxJobsPerFrame)
                break;
        }
    }

    // 6. Only unload when center changes
    if (!m_hasLastCenter || center != m_lastCenter)
    {
        unloadFarChunks(center);
        m_lastCenter = center;
        m_hasLastCenter = true;
    }
}

const Chunk* ChunkManager::getChunk(const ChunkCoord& coord) const
{
    auto it = m_chunks.find(coord);
    if (it == m_chunks.end())
        return nullptr;
    return &it->second;
}

Voxel ChunkManager::getVoxelAt(int wx, int wy, int wz) const
{
    int cx = (int)std::floor((float)wx / Chunk::SIZE);
    int cy = (int)std::floor((float)wy / Chunk::SIZE);
    int cz = (int)std::floor((float)wz / Chunk::SIZE);

    ChunkCoord coord{ cx, cy, cz };

    auto it = m_chunks.find(coord);
    if (it == m_chunks.end())
        return Voxel{ 0 };

    const Chunk& chunk = it->second;
    glm::ivec3 base = chunk.getWorldPosition();

    int lx = wx - base.x;
    int ly = wy - base.y;
    int lz = wz - base.z;

    if (lx < 0 || lx >= Chunk::SIZE ||
        ly < 0 || ly >= Chunk::SIZE ||
        lz < 0 || lz >= Chunk::SIZE)
        return Voxel{ 0 };

    return chunk.get(lx, ly, lz);
}


void ChunkManager::unloadFarChunks(const ChunkCoord& center)
{
    std::vector<ChunkCoord> toRemove;

    for (auto& [coord, chunk] : m_chunks)
    {
        int dx = coord.x - center.x;
        int dy = coord.y - center.y;
        int dz = coord.z - center.z;

        if (std::abs(dx) > m_loadRadius ||
            std::abs(dy) > m_verticalRadius ||
            std::abs(dz) > m_loadRadius)
        {
            toRemove.push_back(coord);
        }
    }

    for (auto& coord : toRemove)
        unloadChunk(coord);
}

void ChunkManager::unloadChunk(const ChunkCoord& coord)
{
    m_chunks.erase(coord);
}

void ChunkManager::workerThread()
{
    while (true)
    {
        ChunkCoord coord;
        {
            std::unique_lock<std::mutex> lock(m_jobMutex);

            m_jobCv.wait(lock, [&]()
            {
                return m_shutdown || !m_jobs.empty();
            });

            if (m_shutdown && m_jobs.empty())
                return;

            coord = m_jobs.front();
            m_jobs.pop();
        }

        // Heavy work off-thread:
        Chunk chunk(glm::ivec3(coord.x, coord.y, coord.z));
        chunk.generateTerrain();

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        ChunkMesher::buildMeshRaw(chunk, *this, vertices, indices);

        ChunkBuildResult result(
            coord,
            std::move(chunk),
            std::move(vertices),
            std::move(indices)
        );

        {
            std::lock_guard<std::mutex> lock(m_resultMutex);
            m_results.push(std::move(result));
        }
    }
}

void ChunkManager::pumpResults()
{
    std::queue<ChunkBuildResult> localResults;

    {
        std::lock_guard<std::mutex> lock(m_resultMutex);
        std::swap(localResults, m_results);
    }

    while (!localResults.empty())
    {
        ChunkBuildResult result = std::move(localResults.front());
        localResults.pop();

        // Erase from in-progress
        m_chunksInProgress.erase(result.coord);

        // Store the chunk
        auto [it, inserted] = m_chunks.emplace(result.coord, std::move(result.chunk));

        // Build RenderObject from raw mesh data
        Render::RenderObject obj = ChunkMesher::buildMeshFromRaw(
            it->second,
            result.vertices,
            result.indices
        );

        m_scene.addObject(std::move(obj));
    }
}
