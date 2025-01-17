#include <device/device.h>
#include <utils/state.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

void init_monitor(int argc, char *argv[]);
void start_debugger();
void close_disasm();

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    init_monitor(argc, argv);
    start_debugger();
    close_device();
#ifdef CONFIG_ENABLE_ITRACE
    close_disasm();
#endif
    return is_good_exit_state() ? EXIT_SUCCESS : EXIT_FAILURE;
}
