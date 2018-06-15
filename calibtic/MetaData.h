#pragma once

#include <ctime>
#include <string>
#include <ostream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

namespace calibtic {

using std::time_t;

class MetaData
{
public:
	MetaData(int id = -1,
			 std::string const& author = std::string(),
			 std::string const& comment = std::string());

	int getID() const;
	void setID(int id);

	std::string getAuthor() const;
	void setAuthor(std::string const& author);

	std::string getComment() const;
	void setComment(std::string const& comment);

	time_t getModifyPOSIX() const;
	std::string getModifyTime() const;

	time_t getCreatePOSIX() const;
	std::string getCreateTime() const;

	void check() const;

	std::ostream& operator<< (std::ostream&) const;

private:
	void updateModify();

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	// Meta Information
	int mId;                //<! unique id
	std::string mAuthor;   //<! author of data set
	std::string mComment;  //<! comment of author

	time_t mCreation;     //<! creation date of set
	time_t mLastModify;   //<! date of last modification
};

std::ostream& operator<< (std::ostream&, MetaData const& md);

} // calibtic



namespace calibtic {

template<typename Archiver>
void MetaData::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("id",      mId)
	   & make_nvp("author",  mAuthor)
	   & make_nvp("comment", mComment)
	   & make_nvp("ctime",   mCreation)
	   & make_nvp("mtime",   mLastModify);
}

}
