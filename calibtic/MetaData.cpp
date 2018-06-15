#include "calibtic/MetaData.h"

#include <stdexcept>
#include <cassert>
#include <boost/lexical_cast.hpp>

//BOOST_CLASS_EXPORT_IMPLEMENT(calibtic::MetaData)

namespace calibtic {

MetaData::MetaData(int id,
				   std::string const& author,
				   std::string const& comment) :
	mId(id),
	mAuthor(author),
	mComment(comment),
	mCreation(),
	mLastModify()
{
	std::time(&mCreation);
	updateModify();
}

int MetaData::getID() const
{
	return mId;
}
void MetaData::setID(int id)
{
	updateModify();
	mId = id;
}

std::string MetaData::getAuthor() const
{
	return mAuthor;
}
void MetaData::setAuthor(std::string const& author)
{
	updateModify();
	mAuthor = author;
}

std::string MetaData::getComment() const
{
	return mComment;
}

void MetaData::setComment(std::string const& comment)
{
	updateModify();
	mComment = comment;
}

time_t MetaData::getModifyPOSIX() const
{
	return mLastModify;
}

std::string MetaData::getModifyTime() const
{
	return boost::lexical_cast<std::string>(mLastModify);
}

time_t MetaData::getCreatePOSIX() const
{
	return mCreation;
}

std::string MetaData::getCreateTime() const
{
	return boost::lexical_cast<std::string>(mCreation);
}

void MetaData::check() const
{
	// check Meta Data
	if (mAuthor.empty() || mId==-1 || !mCreation || !mLastModify) {
		throw std::runtime_error("incomplete tranlation MetaData");
	}
	if (mCreation > mLastModify) {
		throw std::runtime_error("this MetaData object violates causality");
	}
}

std::ostream& MetaData::operator<< (std::ostream& os) const
{
	os << "MetaData: "
		<< "id: " << mId
		<< " author: " << mAuthor;
	if (!mComment.empty()) {
		os << " comment: " << mComment;
	}
	os << " created: " << std::asctime(std::localtime(&mCreation))
		<< " last modified: " << std::asctime(std::localtime(&mLastModify));
	return os;
}

void MetaData::updateModify()
{
	std::time(&mLastModify);
}


std::ostream& operator<< (std::ostream& os, MetaData const& md)
{
	return md.operator<<(os);
}

} // calibtic
