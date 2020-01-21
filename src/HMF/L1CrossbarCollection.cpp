#include "calibtic/HMF/L1CrossbarCollection.h"

#include <vector>
#include <boost/type_traits.hpp>
#include "halco/common/iter_all.h"
#include "calibtic/HMF/L1CrossbarCalibration.h"

namespace HMF {

L1CrossbarCollection::L1CrossbarCollection() {}

L1CrossbarCollection::~L1CrossbarCollection() {}

void L1CrossbarCollection::setDefaults()
{
	for (size_t ii : halco::common::iter_all<halco::hicann::v2::VLineOnHICANN>()) {
		boost::shared_ptr<L1CrossbarCalibration> cb(new L1CrossbarCalibration);
		cb->setDefaults();
		erase(ii);
		insert(ii, cb);
	}
	for (size_t ii : halco::common::iter_all<halco::hicann::v2::HLineOnHICANN>()) {
		boost::shared_ptr<L1CrossbarCalibration> cb(new L1CrossbarCalibration);
		cb->setDefaults();
		erase(ii+VtoH_offset);
		insert(ii+VtoH_offset, cb);
	}
}

template <typename T>
size_t L1CrossbarCollection::getMaxSwitchesPerRow(T line) const
{
	static_assert(
	    boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value ||
	    boost::is_same<halco::hicann::v2::VLineOnHICANN, T>::value);
	bool is_horizontal = boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value;
	size_t address = line.value() + is_horizontal * VtoH_offset;
	if (!exists(address) || !at(address)) {
		throw std::runtime_error("No Calibration data for [H/V]Line ");
	}
	L1CrossbarCalibration const& cb =
	    *boost::dynamic_pointer_cast<L1CrossbarCalibration const>(at(address));
	return cb.getMaxSwitchesPerRow();
}
template size_t L1CrossbarCollection::getMaxSwitchesPerRow<halco::hicann::v2::HLineOnHICANN>(
    halco::hicann::v2::HLineOnHICANN line) const;
template size_t L1CrossbarCollection::getMaxSwitchesPerRow<halco::hicann::v2::VLineOnHICANN>(
    halco::hicann::v2::VLineOnHICANN line) const;

template <typename T>
size_t L1CrossbarCollection::getMaxSwitchesPerColumn(T line) const
{
	static_assert(
	    boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value ||
	    boost::is_same<halco::hicann::v2::VLineOnHICANN, T>::value);
	bool is_horizontal = boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value;
	size_t address = line.value() + is_horizontal * VtoH_offset;

	if (!exists(address) || !at(address)) {
		throw std::runtime_error("No Calibration data for [H/V]Line ");
	}
	L1CrossbarCalibration const& cb =
	    *boost::dynamic_pointer_cast<L1CrossbarCalibration const>(at(address));
	return cb.getMaxSwitchesPerColumn();
}
template size_t L1CrossbarCollection::getMaxSwitchesPerColumn<halco::hicann::v2::HLineOnHICANN>(
    halco::hicann::v2::HLineOnHICANN line) const;
template size_t L1CrossbarCollection::getMaxSwitchesPerColumn<halco::hicann::v2::VLineOnHICANN>(
    halco::hicann::v2::VLineOnHICANN line) const;

template <typename T>
void L1CrossbarCollection::setMaxSwitchesPerRow(T line, size_t const s)
{
	static_assert(
	    boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value ||
	    boost::is_same<halco::hicann::v2::VLineOnHICANN, T>::value);
	bool is_horizontal = boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value;
	size_t address = line.value() + is_horizontal * VtoH_offset;
	if (exists(address) && at(address)) {
		L1CrossbarCalibration& cb = *boost::dynamic_pointer_cast<L1CrossbarCalibration>(at(address));
		cb.setMaxSwitchesPerRow(s);
	} else {
		boost::shared_ptr<L1CrossbarCalibration> cb(new L1CrossbarCalibration);
		cb->setDefaults();
		cb->setMaxSwitchesPerRow(s);
		insert(address, cb);
	}
}
template void L1CrossbarCollection::setMaxSwitchesPerRow<halco::hicann::v2::HLineOnHICANN>(
    halco::hicann::v2::HLineOnHICANN line, size_t const s);
template void L1CrossbarCollection::setMaxSwitchesPerRow<halco::hicann::v2::VLineOnHICANN>(
    halco::hicann::v2::VLineOnHICANN line, size_t const s);

template <typename T>
void L1CrossbarCollection::setMaxSwitchesPerColumn(T line, size_t const s)
{
	static_assert(
	    boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value ||
	    boost::is_same<halco::hicann::v2::VLineOnHICANN, T>::value);
	bool is_horizontal = boost::is_same<halco::hicann::v2::HLineOnHICANN, T>::value;
	size_t address = line.value() + is_horizontal * VtoH_offset;
	if (exists(address) && at(address)) {
		L1CrossbarCalibration& cb = *boost::dynamic_pointer_cast<L1CrossbarCalibration>(at(address));
		cb.setMaxSwitchesPerColumn(s);
	} else {
		boost::shared_ptr<L1CrossbarCalibration> cb(new L1CrossbarCalibration);
		cb->setDefaults();
		cb->setMaxSwitchesPerColumn(s);
		insert(address, cb);
	}
}
template void L1CrossbarCollection::setMaxSwitchesPerColumn<halco::hicann::v2::HLineOnHICANN>(
    halco::hicann::v2::HLineOnHICANN line, size_t const s);
template void L1CrossbarCollection::setMaxSwitchesPerColumn<halco::hicann::v2::VLineOnHICANN>(
    halco::hicann::v2::VLineOnHICANN line, size_t const s);

boost::shared_ptr<L1CrossbarCollection> L1CrossbarCollection::create()
{
	return boost::shared_ptr<L1CrossbarCollection>(new L1CrossbarCollection());
}

void L1CrossbarCollection::copy(Collection const& rhs)
{
	*this = dynamic_cast<L1CrossbarCollection const&>(rhs);
}

} // HMF
