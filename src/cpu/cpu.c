#include <memory.h>
#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <cpu/reg.h>
#include <device/device.h>
#include <device/timer.h>
#include <string.h>
#include <utils/state.h>

CPU_State cpu = {};

static uint64_t step_counter = 0;
static const uint32_t builtin_img[] = {
    0x00000297, // auipc t0,0
    0x00028823, // sb  zero,16(t0)
    0x0102c503, // lbu a0,16(t0)
    0x00100073, // ebreak
    0x00000000
};

static void exec_once() {
    step_counter++;
    // exec the inst
    extern void inst_exec_once(exec_t *);
    exec_t exec_info = { .snpc = cpu.pc, .pc = cpu.pc };
    inst_exec_once(&exec_info);
    cpu.pc = exec_info.dnpc;
}

void init_cpu(bool img_builtin) {
    memset(&cpu, 0, sizeof(cpu));
    cpu.pc = CONFIG_RESET_VECTOR;
    csr(mstatus) = 0x1800;
    Log("CPU Initialized");
    if (img_builtin) {
        Log("Loading builtin image to 0x%" PRIx64 "...", (uint64_t)CONFIG_RESET_VECTOR);
        memcpy(guest_to_host(CONFIG_RESET_VECTOR), builtin_img, sizeof(builtin_img));
    }
}

void cpu_exec(uint64_t step) {
    switch (semu_state.state) {
        case END: case ABORT: case QUIT: stop_timers(); printf("Program has ended. Please restart semu.\n"); return;
        default: SET_STATE(RUNNING); resume_timers(); break;
    }

    for (uint64_t i = 0; i < step; i++) {
        exec_once();
        update_device();
        if (semu_state.state != RUNNING) {
            break;
        }
    }

    switch (semu_state.state) {
        case RUNNING: SET_STATE(STOP); stop_timers(); break;
        case END: Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc); stop_timers(); break;
        case ABORT: Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc); stop_timers(); break;
        case QUIT: stop_timers(); break;
        default: break;
    }
}
