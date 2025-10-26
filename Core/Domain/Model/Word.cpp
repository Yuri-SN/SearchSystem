#include "Word.h"

namespace Core::Domain::Model {
Word::Word(std::string text) : id_(0), text_(std::move(text)) {}

Word::Word(IdType id, std::string text) : id_(id), text_(std::move(text)) {}

Word::IdType Word::getId() const {
    return id_;
}

const std::string& Word::getText() const {
    return text_;
}

bool Word::isPersisted() const {
    return id_ != 0;
}

void Word::setId(IdType id) {
    id_ = id;
}
} // namespace Core::Domain::Model
