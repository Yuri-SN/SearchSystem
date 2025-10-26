#include "Document.h"

namespace Core::Domain::Model {
Document::Document(std::string url, std::string content)
    : id_(0), url_(std::move(url)), content_(std::move(content)) {}

Document::Document(IdType id, std::string url, std::string content)
    : id_(id), url_(std::move(url)), content_(std::move(content)) {}

Document::IdType Document::getId() const {
    return id_;
}

const std::string& Document::getUrl() const {
    return url_;
}

const std::string& Document::getContent() const {
    return content_;
}

bool Document::isPersisted() const {
    return id_ != 0;
}

void Document::setId(IdType id) {
    id_ = id;
}
} // namespace Core::Domain::Model
