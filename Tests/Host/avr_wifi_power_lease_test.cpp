#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "wifi_power_lease.h"

namespace {

void expect(bool condition, const char *name)
{
	if(!condition)
	{
		std::cerr << "FAIL " << name << '\n';
		std::exit(EXIT_FAILURE);
	}

	std::cout << "PASS " << name << '\n';
}

void update_lease_is_bounded_to_five_minutes()
{
	std::uint16_t lease = wifiUpdateLeaseRenew();
	expect(lease == 300U, "update_lease_starts_at_five_minutes");

	bool expiredEarly = false;
	for(std::uint16_t second = 1; second < 300U; second++)
	{
		expiredEarly = expiredEarly || wifiUpdateLeaseTick(&lease);
	}
	expect(!expiredEarly, "update_lease_does_not_expire_early");
	expect(lease == 1U, "update_lease_has_one_final_second");
	expect(wifiUpdateLeaseTick(&lease), "update_lease_expires_at_five_minutes");
	expect(lease == 0U, "expired_update_lease_stays_zero");
	expect(!wifiUpdateLeaseTick(&lease), "expired_update_lease_does_not_repeat");
}

void renewal_and_release_are_explicit()
{
	std::uint16_t lease = 7U;
	lease = wifiUpdateLeaseRenew();
	expect(lease == WIFI_UPDATE_SHUTDOWN_SECONDS, "renewal_restores_bounded_update_lease");
	wifiUpdateLeaseRelease(&lease);
	expect(lease == 0U, "ordinary_activity_releases_update_lease");
	expect(
		WIFI_NORMAL_SHUTDOWN_SECONDS < WIFI_UPDATE_SHUTDOWN_SECONDS,
		"normal_shutdown_remains_shorter_than_update_lease");
}

} // namespace

int main()
{
	update_lease_is_bounded_to_five_minutes();
	renewal_and_release_are_explicit();

	std::cout << "All AVR WiFi power lease tests passed\n";
	return EXIT_SUCCESS;
}
