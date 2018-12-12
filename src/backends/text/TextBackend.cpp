#include "calibtic/backends/text/TextBackend.h"

// polymorphic classes need to be registered in each backend
#include "calibtic/backends/export.ipp"

#include "calibtic/backend/interface.h"

#include <stdexcept>
#include <iostream>


namespace calibtic {
namespace backend {

log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Calibtic");


TextBackend::TextBackend() :
	mPath(".") {}

TextBackend::~TextBackend() {}

TextBackend::path& TextBackend::getPath()
{
	return mPath;
}

TextBackend::path const& TextBackend::getPath() const
{
	return mPath;
}

void TextBackend::init()
{
	namespace fs = boost::filesystem;
	if (exists("path")) {
		getPath() = get<std::string>("path");
		if (!fs::is_directory(getPath())) {
			std::stringstream err;
			err << "TextBackend::init(): path " << getPath() << " not available";
			throw std::runtime_error(err.str());
		}
	}
}

void TextBackend::load(
	std::string const& id,
	MetaData& metadata,
	Collection& c)
{
	LOG4CXX_DEBUG(logger, "Load Collection");
	boost::shared_ptr<Collection> ptr;
	load("Collection", id, metadata, ptr);
	c.copy(*ptr);
}

void TextBackend::load(
	std::string const& id,
	MetaData& metadata,
	Calibration& c)
{
	LOG4CXX_DEBUG(logger, "Load Calibration");
	boost::shared_ptr<Calibration> ptr;
	load("Calibration", id, metadata, ptr);
	c.copy(*ptr);
}

void TextBackend::load(std::string const& id,
	 MetaData& metadata,
	 boost::shared_ptr<Calibration> & ptr)
{
	LOG4CXX_DEBUG(logger, "Load Collection");
	load("Collection", id, metadata, ptr);
}

void TextBackend::store(
	std::string const& id,
	MetaData const& metadata,
	Collection const& set)
{
	using boost::serialization::null_deleter;
	boost::shared_ptr<Collection const> ptr(&set, null_deleter());
	store("Collection", id, metadata, ptr);
}

void TextBackend::store(
	std::string const& id,
	MetaData const& metadata,
	Calibration const& set)
{
	using boost::serialization::null_deleter;
	boost::shared_ptr<Calibration const> ptr(&set, null_deleter());
	store("Calibration", id, metadata, ptr);
}

void TextBackend::store(std::string const& id,
	 MetaData const& metadata,
	 boost::shared_ptr<const Calibration> ptr)
{
	store("Calibration", id, metadata, ptr);
}

TextBackend::path
TextBackend::getFilename(
	std::string const& id,
	MetaData const&) const
{
	return getPath() / (id + ".txt");
}

} // backend
} // calibtic


extern "C" {

backend_t* createBackend()
{
	return new calibtic::backend::TextBackend();
}

void destroyBackend(backend_t* backend)
{
	delete backend;
}

} // extern "C"
