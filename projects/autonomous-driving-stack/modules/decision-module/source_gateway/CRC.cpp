#if !defined(USE_PRIVATE_IMPL)
#include <cstdio>
#include <cstdlib>

/**
 * @brief Short description of `main`.
 *
 * Detailed explanation of what `main` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
int main()
{
    std::fprintf(
        stderr,
        "[decision-module:crc] private implementation is not available. "
        "Set USE_PRIVATE_IMPL=1 and provide modules/common/src/private/decision-module/CRC_impl.cpp\n");
    return EXIT_FAILURE;
}
#endif
