#ifndef SEARCHFILES_SEARCHRESULTS_H
#define SEARCHFILES_SEARCHRESULTS_H

#include <string>
#include <vector>

namespace Search {

    class SearchResult {
    private:
        std::wstring path;
        std::wstring title;
        std::uint32_t fsSize;
        float score;

    public:
        const std::wstring &getPath() const;

        void setPath(const std::wstring &id);

        const std::wstring &getTitle() const;

        void setTitle(const std::wstring &id);

        const std::uint32_t &getFsSize() const;

        void setFsSize(const std::uint32_t &sz);

        const float &getScore() const;

        void setScore(const float &docScore);
    };

    class SearchResults {
    private:
        unsigned int totalHits{0};
        std::vector<SearchResult> results;

    public:
        SearchResults() = default;

        unsigned int getTotalHits() const;

        void setTotalHits(unsigned int hits);

        const std::vector<SearchResult> &getResults() const;

        void appendResult(const SearchResult &result);

        void print() const;
    };

}
#endif // SEARCHFILES_SEARCHRESULTS_H
