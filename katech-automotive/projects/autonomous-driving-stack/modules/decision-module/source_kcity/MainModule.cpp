#if !defined(USE_PRIVATE_IMPL)
#include <cstdio>
#include <cstdlib>

/**
 * @brief Short description of `main`.
 *
 * Detailed explanation of what `main` does,
 * including major steps and responsibilities.
 *
 * @param argc Input parameter.
 * @param argv Input parameter.
 * @return Output value from the function.
 */
int main(int argc, const char *argv[])
{
    (void)argc;
    (void)argv;
    std::fprintf(
        stderr,
        "[decision-module:kcity] private implementation is not available. "
        "Set USE_PRIVATE_IMPL=1 and provide modules/common/src/private/decision-module/*_impl.cpp\n");
    return EXIT_FAILURE;
}
#endif
