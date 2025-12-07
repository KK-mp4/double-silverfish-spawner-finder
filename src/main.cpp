#include <iostream>

int run_stronghold_finder(uint64_t startSeed);

static void print_usage(const char *prog)
{
    std::cerr << "Usage: " << prog << " <startSeed>\n";
    std::cerr << "Examples:\n";
    std::cerr << "  " << prog << " 0\n";
    std::cerr << "  " << prog << " 123456789\n";
}

int main(int argc, char **argv)
{
    uint64_t startSeed = 0;

    if (argc >= 2)
    {
        char *endptr = nullptr;
        startSeed = (uint64_t)strtoull(argv[1], &endptr, 10);
        if (endptr && *endptr != '\0')
        {
            std::cerr << "Invalid starting seed: " << argv[1] << "\n";
            print_usage(argv[0]);
            return 2;
        }
    }

    return run_stronghold_finder(startSeed);
}
