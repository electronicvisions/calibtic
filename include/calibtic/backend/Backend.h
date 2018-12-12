#pragma once

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include "calibtic/MetaData.h"

namespace calibtic {

// fwd decls
class Collection;
class Calibration;

namespace backend {

class Library; // fwd decl

/// Abstract base class for all storage backends
class Backend
{
public:
	virtual ~Backend() {}

	void config(std::string const& key,
				std::string const& val);

	void config(std::string const& key,
				int const val);

	// explicit for py++
	void config_int(std::string const& key,
				int const val);

	void config(std::string const& key,
				double const val);

	virtual void init() = 0;

	// necessary for py++ wrappings
	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 Collection&) = 0;

	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 Calibration&) = 0;

	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 boost::shared_ptr<Calibration> & ptr) = 0;

	virtual void
	store(std::string const& id,
		  MetaData const& metadata,
		  Collection const&) = 0;

	virtual void
	store(std::string const& id,
		  MetaData const& metadata,
		  Calibration const&) = 0;

	virtual void
	store(std::string const& id,
		 MetaData const& metadata,
		 boost::shared_ptr<const Calibration> ptr) = 0;

protected:
	typedef boost::variant<std::string, double, int> config_value_t;
	typedef std::map<std::string, config_value_t> config_map_t;

	void config_(std::string const& key,
				config_value_t const& val);

	// returns true or falls wheter or not key exists in map
	bool exists(std::string const& key) const;

	template<typename T>
	T& get(std::string const& key);

	template<typename T>
	T const& get(std::string const& key) const;

#ifndef PYPLUSPLUS
	// returns the number of provided keys existant in the map
	template<typename ... Keys>
	int exist(Keys const& ... keys) const;

private:
	template<typename Return>
	Return sum() const;

	template<typename Return, typename T, typename ... Ts>
	Return sum(T const& t, Ts const& ... ts) const;
#endif

	config_map_t  mConfig;
};

boost::shared_ptr<Backend>
loadBackend(boost::shared_ptr<Library> lib);

} // backend
} // calibtic




// implementations
namespace calibtic {
namespace backend {

template<typename T>
T& Backend::get(std::string const& key)
{
	return boost::get<T>(mConfig.at(key));
}

template<typename T>
T const& Backend::get(std::string const& key) const
{
	return boost::get<T>(mConfig.at(key));
}

#ifndef PYPLUSPLUS
template<typename ... Keys>
int Backend::exist(Keys const& ... keys) const
{
	return sum<int>(exists(std::string(keys))...);
}

template<typename Return>
inline
Return Backend::sum() const
{
	return 0;
}

template<typename Return, typename T, typename ... Ts>
inline
Return Backend::sum(T const& t, Ts const& ... ts) const
{
	return static_cast<Return>(t) + sum<Return>(ts...);
}
#endif // PYPLUSPLUS

} // backend
} // calibtic
