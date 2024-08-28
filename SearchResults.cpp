#include <iostream>
#include <cstdint>
#include "SearchResults.h"

namespace Search {

    const std::wstring &SearchResult::getPath() const {
        return path;
    }

    void SearchResult::setPath(const std::wstring &docPath) {
        path = docPath;
    }

    const std::wstring &SearchResult::getTitle() const {
        return title;
    }

    void SearchResult::setTitle(const std::wstring &docTitle) {
        title = docTitle;
    }

    const std::uint32_t &SearchResult::getFsSize() const {
        return fsSize;
    }

    void SearchResult::setFsSize(const std::uint32_t &sz) {
        fsSize = sz;
    }

    const float &SearchResult::getScore() const {
        return score;
    };

    void SearchResult::setScore(const float &docScore) {
        score = docScore;
    }

    const std::wstring &SearchResult::getExtension() const {
        return extension;
    }

    void SearchResult::setExtension(const std::wstring &docExtension) {
        extension = docExtension;
    }

    ///

    void SearchResults::print() const {
        std::cout << "Hits: " << totalHits << "\n";
    }

    unsigned int SearchResults::getTotalHits() const {
        return totalHits;
    }

    void SearchResults::setTotalHits(unsigned int hits) {
        totalHits = hits;
    }

    const std::vector<SearchResult> &SearchResults::getResults() const {
        return results;
    }

    void SearchResults::appendResult(const SearchResult &result) {
        results.push_back(result);
    }
}