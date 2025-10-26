#include "Url.h"

#include <regex>

namespace Core::Domain::ValueObject {
std::optional<Url> Url::create(const std::string& url) {
    std::string scheme, host, path;

    if (!parse(url, scheme, host, path)) {
        return std::nullopt;
    }

    return Url(url, std::move(scheme), std::move(host), std::move(path));
}

const std::string& Url::toString() const {
    return url_;
}

const std::string& Url::getScheme() const {
    return scheme_;
}

const std::string& Url::getHost() const {
    return host_;
}

const std::string& Url::getPath() const {
    return path_;
}

bool Url::operator==(const Url& other) const {
    return url_ == other.url_;
}

bool Url::operator!=(const Url& other) const {
    return !(*this == other);
}

Url::Url(std::string url, std::string scheme, std::string host, std::string path)
    : url_(std::move(url)),
      scheme_(std::move(scheme)),
      host_(std::move(host)),
      path_(std::move(path)) {}

bool Url::parse(const std::string& url, std::string& scheme, std::string& host, std::string& path) {
    // Простая валидация URL с помощью регулярного выражения
    // Формат: scheme://host/path
    std::regex urlRegex(R"(^(https?)://([^/]+)(/.*)?$)");
    std::smatch match;

    if (!std::regex_match(url, match, urlRegex)) {
        return false;
    }

    scheme = match[1].str();
    host = match[2].str();
    path = match[3].matched ? match[3].str() : "/";

    return true;
}
} // namespace Core::Domain::ValueObject
