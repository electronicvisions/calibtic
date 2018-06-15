#include "backends/xml/XMLBackend.h"

// polymorphic classes need to be registered in each backend
#include "backends/export.ipp"

#include "calibtic/backend/interface.h"

#include <stdexcept>
#include <iostream>


namespace calibtic {
namespace backend {

log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Calibtic");


XMLBackend::XMLBackend() :
	mPath(".") {}

XMLBackend::~XMLBackend() {}

XMLBackend::path& XMLBackend::getPath()
{
	return mPath;
}

XMLBackend::path const& XMLBackend::getPath() const
{
	return mPath;
}

void XMLBackend::init()
{
	namespace fs = boost::filesystem;
	if (exists("path")) {
		getPath() = get<std::string>("path");
		if (!fs::is_directory(getPath())) {
			std::stringstream err;
			err << "XMLBackend::init(): path " << getPath() << " not available";
			throw std::runtime_error(err.str());
		}
	}
}

void XMLBackend::load(
	std::string const& id,
	MetaData& metadata,
	Collection& c)
{
	LOG4CXX_DEBUG(logger, "Load Collection");
	boost::shared_ptr<Collection> ptr;
	load("Collection", id, metadata, ptr);
	c.copy(*ptr);
}

void XMLBackend::load(
	std::string const& id,
	MetaData& metadata,
	Calibration& c)
{
	LOG4CXX_DEBUG(logger, "Load Calibration");
	boost::shared_ptr<Calibration> ptr;
	load("Calibration", id, metadata, ptr);
	c.copy(*ptr);
}

void XMLBackend::load(std::string const& id,
	 MetaData& metadata,
	 boost::shared_ptr<Calibration> & ptr)
{
	LOG4CXX_DEBUG(logger, "Load Collection");
	load("Collection", id, metadata, ptr);
}

void XMLBackend::store(
	std::string const& id,
	MetaData const& metadata,
	Collection const& set)
{
	using boost::serialization::null_deleter;
	boost::shared_ptr<Collection const> ptr(&set, null_deleter());
	store("Collection", id, metadata, ptr);
}

void XMLBackend::store(
	std::string const& id,
	MetaData const& metadata,
	Calibration const& set)
{
	using boost::serialization::null_deleter;
	boost::shared_ptr<Calibration const> ptr(&set, null_deleter());
	store("Calibration", id, metadata, ptr);
}

void XMLBackend::store(std::string const& id,
	 MetaData const& metadata,
	 boost::shared_ptr<const Calibration> ptr)
{
	store("Calibration", id, metadata, ptr);
}

XMLBackend::path
XMLBackend::getFilename(
	std::string const& id,
	MetaData const&) const
{
	return getPath() / (id + ".xml");
}

} // backend
} // calibtic


extern "C" {

backend_t* createBackend()
{
	return new calibtic::backend::XMLBackend();
}

void destroyBackend(backend_t* backend)
{
	delete backend;
}

} // extern "C"
