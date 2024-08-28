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
        std::wstring extension;
        std::vector<std::wstring> highlights;

    public:
        const std::wstring &getPath() const;

        void setPath(const std::wstring &docPath);

        const std::wstring &getTitle() const;

        void setTitle(const std::wstring &id);

        const std::uint32_t &getFsSize() const;

        void setFsSize(const std::uint32_t &sz);

        const float &getScore() const;

        void setScore(const float &docScore);

        const std::wstring &getExtension() const;

        void setExtension(const std::wstring &docExtension);

        const std::vector<std::wstring> &getHighlights() const;

        void setHighlights(const std::vector<std::wstring> &highlights);
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
