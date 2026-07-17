#include "linkbus_command_transaction.h"

#include <cstdlib>
#include <iostream>
#include <limits>

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
	LinkbusCommandTransaction transaction = linkbusCommandTransactionBegin(1000);
	expect(
		linkbusCommandTransactionObserve(&transaction, 1000, false, false, false, 0) ==
			LINKBUS_COMMAND_WAITING,
		"queued_command_is_waiting");
	expect(
		linkbusCommandTransactionObserve(&transaction, 1001, true, true, false, 0) ==
			LINKBUS_COMMAND_WAITING,
		"pending_command_is_waiting");
	expect(
		linkbusCommandTransactionObserve(&transaction, 1002, true, false, false, 0) ==
			LINKBUS_COMMAND_ACKNOWLEDGED,
		"empty_queue_and_cleared_pending_is_acknowledged");

	transaction = linkbusCommandTransactionBegin(1500);
	expect(
		linkbusCommandTransactionObserve(&transaction, 1501, false, false, false, 0) ==
			LINKBUS_COMMAND_WAITING,
		"additional_queued_work_cannot_look_acknowledged");

	transaction = linkbusCommandTransactionBegin(2000);
	expect(
		linkbusCommandTransactionObserve(&transaction, 2001, false, false, false, 1) ==
			LINKBUS_COMMAND_NACKED,
		"nack_fails_command_even_with_queued_work");

	transaction = linkbusCommandTransactionBegin(3000);
	expect(
		linkbusCommandTransactionObserve(&transaction, 6000, true, true, true, 0) ==
			LINKBUS_COMMAND_WAITING,
		"retry_waits_for_pending_command_to_finish");
	expect(
		linkbusCommandTransactionObserve(&transaction, 6001, false, false, true, 0) ==
			LINKBUS_COMMAND_ACK_TIMEOUT,
		"ack_after_retry_rejects_queued_followup_work");

	transaction = linkbusCommandTransactionBegin(4000);
	expect(
		linkbusCommandTransactionObserve(
			&transaction,
			4000 + LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS - 1,
			true,
			true,
			false,
			0) == LINKBUS_COMMAND_WAITING,
		"local_deadline_is_not_early");
	expect(
		linkbusCommandTransactionObserve(
			&transaction,
			4000 + LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS,
			true,
			true,
			false,
			0) == LINKBUS_COMMAND_LOCAL_TIMEOUT,
		"local_deadline_is_bounded");

	transaction = linkbusCommandTransactionBegin(
		std::numeric_limits<uint32_t>::max() - 1000U);
	expect(
		linkbusCommandTransactionObserve(
			&transaction,
			LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS - 1002U,
			true,
			true,
			false,
			0) == LINKBUS_COMMAND_WAITING,
		"millis_wrap_preserves_pre_deadline_wait");
	expect(
		linkbusCommandTransactionObserve(
			&transaction,
			LINKBUS_COMMAND_LOCAL_DEADLINE_MILLIS - 1001U,
			true,
			true,
			false,
			0) == LINKBUS_COMMAND_LOCAL_TIMEOUT,
		"millis_wrap_preserves_deadline");

	expect(
		linkbusCommandTransactionObserve(nullptr, 0, true, false, false, 0) ==
			LINKBUS_COMMAND_INVALID,
		"null_transaction_is_rejected");

	std::cout << "All ESP Linkbus command transaction tests passed\n";
	return 0;
}
