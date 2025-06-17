#include "../include/signal.hpp"
#include <iostream>

// Global variable definition
SignalState g_signal_state;

void handle_sigint(int signum) {
	(void)signum;
	g_signal_state.sigint = 1;
}

void handle_sigterm(int signum) {
	(void)signum;
	g_signal_state.sigterm = 1;
}

void setup_signals() {
	g_signal_state.sigint = 0;
	g_signal_state.sigterm = 0;

	signal(SIGINT, handle_sigint);
	signal(SIGTERM, handle_sigterm);
}
