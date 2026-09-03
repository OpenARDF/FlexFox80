#include "clone_diagnostic_state.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

namespace
{
void expect(bool condition, const char *name)
{
	if (!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(1);
	}
	std::cout << "PASS " << name << '\n';
}
} // namespace

int main()
{
	CloneDiagnosticState state;
	cloneDiagnosticReset(&state);
	expect(!state.active && state.phase == CLONE_DIAG_IDLE, "reset_is_idle");
	cloneDiagnosticBegin(&state, 0);
	expect(cloneDiagnosticElapsedMillis(&state, 250) == 250, "zero_millis_start_is_reported");

	cloneDiagnosticBegin(&state, 1000);
	expect(state.active, "begin_marks_session_active");
	expect(state.phase == CLONE_DIAG_MASTER_ASSOCIATION, "begin_records_association_phase");
	expect(!cloneDiagnosticShouldRemoveIncompleteTemporaryFile(&state), "begin_has_no_incomplete_temporary_file");

	cloneDiagnosticTemporaryFileOpened(&state);
	expect(cloneDiagnosticShouldRemoveIncompleteTemporaryFile(&state), "open_temporary_file_requires_failure_cleanup");
	cloneDiagnosticTemporaryFileCleanupFinished(&state, true);
	expect(!cloneDiagnosticShouldRemoveIncompleteTemporaryFile(&state), "cleanup_clears_incomplete_temporary_file_state");
	expect(state.temporaryFileCleanupAttempted, "temporary_file_cleanup_attempt_is_recorded");
	expect(state.temporaryFileCleanupConfirmed, "temporary_file_cleanup_success_is_recorded");

	cloneDiagnosticTemporaryFileOpened(&state);
	cloneDiagnosticTemporaryFileCompleted(&state);
	expect(!cloneDiagnosticShouldRemoveIncompleteTemporaryFile(&state), "complete_temporary_file_is_preserved_for_validation");
	expect(!state.temporaryFileCleanupAttempted, "normal_eof_does_not_report_cleanup_attempt");

	state.attempt = 2;
	cloneDiagnosticSetPhase(&state, CLONE_DIAG_CLOCK_READY, 2500);
	expect(std::string(cloneDiagnosticPhaseText(state.phase)) == "clock-ready", "phase_has_stable_wire_text");
	expect(cloneDiagnosticElapsedMillis(&state, 3000) == 2000, "session_elapsed_is_reported");
	expect(cloneDiagnosticPhaseElapsedMillis(&state, 3000) == 500, "phase_elapsed_is_reported");

	cloneDiagnosticFail(&state, 3500);
	expect(!state.active && state.phase == CLONE_DIAG_FAILED, "failure_ends_active_session");
	expect(state.failurePhase == CLONE_DIAG_CLOCK_READY, "failure_retains_original_phase");
	expect(cloneDiagnosticElapsedMillis(&state, 9000) == 2500, "failure_freezes_session_elapsed");
	expect(cloneDiagnosticPhaseElapsedMillis(&state, 9000) == 0, "failure_freezes_terminal_phase_elapsed");

	cloneDiagnosticBegin(&state, std::numeric_limits<uint32_t>::max() - 499U);
	cloneDiagnosticSetPhase(&state, CLONE_DIAG_FILE_RECEIVE, std::numeric_limits<uint32_t>::max() - 99U);
	expect(cloneDiagnosticElapsedMillis(&state, 500) == 1000, "elapsed_survives_millis_wrap");
	expect(cloneDiagnosticPhaseElapsedMillis(&state, 500) == 600, "phase_elapsed_survives_millis_wrap");

	state.cleanupConfirmed = true;
	cloneDiagnosticComplete(&state, 1000);
	expect(!state.active && state.phase == CLONE_DIAG_COMPLETE, "completion_retains_cleanup_result");
	expect(state.cleanupConfirmed, "completion_preserves_cleanup_confirmation");
	expect(cloneDiagnosticElapsedMillis(&state, 9000) == 1500, "completion_freezes_wrapped_elapsed");

	std::cout << "All ESP clone diagnostic state tests passed\n";
	return 0;
}
