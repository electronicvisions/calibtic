#pragma once

#include <stdexcept>


class OutsideDomainException : public std::runtime_error {
	public:
		OutsideDomainException(const std::string& msg = "Value outside of domain.") : std::runtime_error(msg) { }
};

namespace calibtic {
} // calibtic
