#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <signal.h>

struct SignalState {
	sig_atomic_t sigint;
	sig_atomic_t sigterm;
};

extern SignalState g_signal_state;

void setup_signals();
void handle_sigint(int signum);
void handle_sigterm(int signum);

#endif
