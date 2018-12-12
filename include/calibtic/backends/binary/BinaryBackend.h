#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>

#include <log4cxx/logger.h>

#include "calibtic/backend/Backend.h"

namespace calibtic {
namespace backend {

extern log4cxx::LoggerPtr logger;

class BinaryBackend :
	public Backend
{
public:
	BinaryBackend();
	virtual ~BinaryBackend();

	virtual void init();

	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 Collection&);

	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 Calibration&);

	virtual void
	load(std::string const& id,
		 MetaData& metadata,
		 boost::shared_ptr<Calibration> & ptr);

	virtual void
	store(std::string const& id,
		  MetaData const& metadata,
		  Collection const&);

	virtual void
	store(std::string const& id,
		  MetaData const& metadata,
		  Calibration const&);

	virtual void
	store(std::string const& id,
		 MetaData const& metadata,
		 boost::shared_ptr<const Calibration> ptr);

private:
	typedef boost::filesystem::path path;

	template<typename T>
	void load(char const* label,
			  std::string const& id,
			  MetaData& metadata,
			  T& t);

	template<typename T>
	void store(char const* label,
			   std::string const& id,
			   MetaData const& metadata,
			   T const&);

	path&       getPath();
	path const& getPath() const;

	path
	getFilename(std::string const& id,
				MetaData const& metadata) const;

	path  mPath;
};

} // backend
} // calibtic



// implementations

namespace calibtic {
namespace backend {

template<typename T>
void BinaryBackend::load(char const* label,
					  std::string const& id,
					  MetaData& metadata,
					  T& t)
{
	namespace fs = boost::filesystem;
	auto file = getFilename(id, metadata);
	if (!fs::exists(file)) {
		LOG4CXX_ERROR(logger, "Calibration file does not exist: " << file);
		throw std::runtime_error(
		    std::string("data set not found: ") + file.string());
	}

	std::fstream stream(file.string(), std::ios::in | std::ios::binary);

	boost::archive::binary_iarchive ia(stream);

	ia >> boost::serialization::make_nvp("metadata", metadata);
	ia >> boost::serialization::make_nvp(label, t);
}

template<typename T>
void BinaryBackend::store(char const* label,
					   std::string const& id,
					   MetaData const& metadata,
					   T const& t)
{
	auto file = getFilename(id, metadata);
	std::fstream stream(file.string(), std::ios::out | std::ios::binary);

	boost::archive::binary_oarchive oa(stream);

	oa << boost::serialization::make_nvp("metadata", metadata);
	oa << boost::serialization::make_nvp(label, t);
}

} // backend
} // calibtic
