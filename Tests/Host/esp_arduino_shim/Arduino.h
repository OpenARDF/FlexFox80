#pragma once

#include <cstddef>
#include <string>
#include <utility>

class String
{
public:
	String() = default;
	String(const char *value) : value_(value ? value : "") {}
	String(const String&) = default;
	String(String&& other) noexcept : value_(std::move(other.value_)) { other.value_.clear(); }
	String& operator=(const String&) = default;
	String& operator=(String&& other) noexcept
	{
		value_ = std::move(other.value_);
		other.value_.clear();
		return *this;
	}

	bool operator==(const char *value) const { return value_ == (value ? value : ""); }
	const char *c_str() const { return value_.c_str(); }
	std::size_t capacity() const { return value_.capacity(); }

private:
	std::string value_;
};
