#include "WordFrequency.h"

namespace Core::Domain::Model {
WordFrequency::WordFrequency(DocumentIdType documentId, WordIdType wordId, FrequencyType frequency)
    : documentId_(documentId), wordId_(wordId), frequency_(frequency) {}

WordFrequency::DocumentIdType WordFrequency::getDocumentId() const {
    return documentId_;
}

WordFrequency::WordIdType WordFrequency::getWordId() const {
    return wordId_;
}

WordFrequency::FrequencyType WordFrequency::getFrequency() const {
    return frequency_;
}

void WordFrequency::setFrequency(FrequencyType frequency) {
    frequency_ = frequency;
}
} // namespace Core::Domain::Model
