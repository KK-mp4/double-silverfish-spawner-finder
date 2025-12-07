#include <config.hpp>

int run_stronghold_finder(uint64_t startSeed = 0)
{
    printf("Starting search from seed %llu...\n", startSeed);

    std::ofstream log("logs/finder.log", std::ios::app);
    if (!log)
        fprintf(stderr, "Failed to open log file 'logs/finder.log'\n");
    else
        log << "\n\nseed,\tdistance,\tversion_1,\tworld_x_1,\tworld_z_1,\tversion_2,\tworld_x_2,\tworld_z_2\n";

    unsigned int smallestSq = std::numeric_limits<int>::max();

    const unsigned int numThreads = std::max(1u, std::thread::hardware_concurrency());

    // Atomic seed provider
    std::atomic<uint64_t> nextSeed(startSeed);
    std::atomic<uint64_t> processedSeeds(0);
    std::mutex bestMutex;

    // Worker lambda
    auto worker = [&](unsigned int workerId)
    {
        Generator g;
        StrongholdIter sh;
        Stronghold strongholdA, strongholdB;
        unsigned int numStrongholds = 0;

        while (true)
        {
            uint64_t seed = nextSeed.fetch_add(1, std::memory_order_relaxed);

            std::vector<Stronghold> strongholds;

            for (int version : MC_VERSIONS)
            {
                if (version < MC_1_0_0)
                    continue;

                initFirstStronghold(&sh, version, seed);
                setupGenerator(&g, version, 0);
                applySeed(&g, DIM_OVERWORLD, seed);

                if (version < MC_1_9)
                    numStrongholds = 3;
                else if (version >= MC_1_9)
                    numStrongholds = 128;

                for (unsigned int i = 1; i <= numStrongholds; ++i)
                {
                    if (nextStronghold(&sh, &g) <= 0)
                        break;

                    Stronghold stronghold{version, sh.pos};
                    strongholds.push_back(stronghold);
                }
            }

            unsigned int seedSmallestSq = std::numeric_limits<int>::max();

            for (size_t a = 0; a < strongholds.size(); ++a)
            {
                for (size_t b = a + 1; b < strongholds.size(); ++b)
                {
                    int dx = strongholds[a].pos.x - strongholds[b].pos.x;
                    int dz = strongholds[a].pos.z - strongholds[b].pos.z;
                    unsigned int sq = dx * dx + dz * dz;

                    if (sq == 0)
                        continue;
                    else if (sq <= seedSmallestSq)
                    {
                        // new global minimum found
                        seedSmallestSq = sq;
                        strongholdA = strongholds[a];
                        strongholdB = strongholds[b];
                    }
                    else if (sq <= THRESHOLD_SQ)
                    {
                        printf("[GOOD] seed=%llu distance=%.2f version=%d /tp @p %d 100 %d and version=%d /tp @p %d 100 %d\n", seed, std::sqrt(sq), strongholds[a].version, strongholds[a].pos.x, strongholds[a].pos.z, strongholds[b].version, strongholds[b].pos.x, strongholds[b].pos.z);
                        fflush(stdout);

                        if (log)
                        {
                            log << seed << ",\t" << std::sqrt(sq) << ",\t" << strongholds[a].version << ",\t" << strongholds[a].pos.x << ",\t" << strongholds[a].pos.z << ",\t"  << strongholds[b].version << strongholds[b].pos.x << ",\t" << strongholds[b].pos.z << "\n";

                            log.flush();
                        }
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(bestMutex);

                if (seedSmallestSq <= smallestSq)
                {
                    smallestSq = seedSmallestSq;
                    printf("[NEW BEST] seed=%llu distance=%.2f version=%d /tp @p %d 100 %d and version=%d /tp @p %d 100 %d\n", seed, std::sqrt(seedSmallestSq), strongholdA.version, strongholdA.pos.x, strongholdA.pos.z, strongholdB.version, strongholdB.pos.x, strongholdB.pos.z);
                    fflush(stdout);

                    if (log)
                    {
                        log << seed << ",\t" << std::sqrt(seedSmallestSq) << ",\t" << strongholdA.version << ",\t" << strongholdA.pos.x << ",\t" << strongholdA.pos.z << ",\t" << strongholdB.version << ",\t" << strongholdB.pos.x << ",\t" << strongholdB.pos.z << "\n";

                        log.flush();
                    }
                }
            }

            uint64_t done = processedSeeds.fetch_add(1, std::memory_order_relaxed) + 1;
            if (done % PRINT_PROGRESS_EVERY_SEEDS == 0)
            {
                std::lock_guard<std::mutex> lock(bestMutex);
                printf("[PROGRESS] worker=%u processed-seeds=%llu\n",
                       workerId, (uint64_t)done);
                fflush(stdout);
            }
        }
    };

    // Spawn workers
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    for (unsigned int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back(worker, t);
    }

    for (auto &th : threads)
        th.join();

    printf("Done.\n");
    return 0;
}
