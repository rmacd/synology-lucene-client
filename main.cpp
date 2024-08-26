#include "lucene++/targetver.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "lucene++/LuceneHeaders.h"
#include "lucene++/FilterIndexReader.h"
#include "lucene++/MiscUtils.h"
#include "SynoTerms.h"

/// Use the norms from one field for all fields.  Norms are read into memory, using a byte of memory
/// per document per searched field.  This can cause search of large collections with a large number
/// of fields to run out of memory.  If all of the fields contain only a single token, then the norms
/// are all identical, then single norm vector may be shared.
class OneNormsReader : public Lucene::FilterIndexReader {
public:
    OneNormsReader(const Lucene::IndexReaderPtr& in, const Lucene::String& field) : FilterIndexReader(in) {
        this->field = field;
    }

    virtual ~OneNormsReader() {
    }

protected:
    Lucene::String field;

public:
    virtual Lucene::ByteArray norms(const Lucene::String& field) {
        return in->norms(this->field);
    }
};

/// This demonstrates a typical paging search scenario, where the search engine presents pages of size n
/// to the user. The user can then go to the next page if interested in the next hits.
///
/// When the query is executed for the first time, then only enough results are collected to fill 5 result
/// pages. If the user wants to page beyond this limit, then the query is executed another time and all
/// hits are collected.
static void doPagingSearch(const Lucene::SearcherPtr& searcher, const Lucene::QueryPtr& query, int32_t hitsPerPage, bool raw, bool interactive) {
    // Collect enough docs to show 5 pages
    Lucene::TopScoreDocCollectorPtr collector = Lucene::TopScoreDocCollector::create(5 * hitsPerPage, false);
    searcher->search(query, collector);
    Lucene::Collection<Lucene::ScoreDocPtr> hits = collector->topDocs()->scoreDocs;

    int32_t numTotalHits = collector->getTotalHits();
    std::wcout << numTotalHits << L" total matching documents\n";

    int32_t start = 0;
    int32_t end = std::min(numTotalHits, hitsPerPage);

    while (true) {
        if (end > hits.size()) {
            std::wcout << L"Only results 1 - " << hits.size() << L" of " << numTotalHits << L" total matching documents collected.\n";
            std::wcout << L"Collect more (y/n) ?";
            Lucene::String line;
            std::wcin >> line;
            boost::trim(line);

            if (line.empty() || boost::starts_with(line, L"n")) {
                break;
            }

            collector = Lucene::TopScoreDocCollector::create(numTotalHits, false);
            searcher->search(query, collector);
            hits = collector->topDocs()->scoreDocs;
        }

        end = std::min(hits.size(), start + hitsPerPage);

        for (int32_t i = start; i < end; ++i) {
            if (raw) { // output raw format
                std::wcout << L"doc=" << hits[i]->doc << L" score=" << hits[i]->score << L"\n";
                continue;
            }

            Lucene::DocumentPtr doc = searcher->doc(hits[i]->doc);
            Lucene::String path = doc->get(SynoTerms::path);
            if (!path.empty()) {
                std::wcout << Lucene::StringUtils::toString(i + 1) + L". " << path << L"\n";
                Lucene::String title = doc->get(SynoTerms::title);
                if (!title.empty()) {
                    std::wcout << L"   Title: " << doc->get(SynoTerms::title) << L"\n";
                }
            } else {
                std::wcout << Lucene::StringUtils::toString(i + 1) + L". No path for this document\n";
            }
        }

        if (!interactive) {
            break;
        }

        if (numTotalHits >= end) {
            bool quit = false;
            while (true) {
                std::wcout << L"Press ";
                if (start - hitsPerPage >= 0) {
                    std::wcout << L"(p)revious page, ";
                }
                if (start + hitsPerPage < numTotalHits) {
                    std::wcout << L"(n)ext page, ";
                }
                std::wcout << L"(q)uit or enter number to jump to a page: ";

                Lucene::String line;
                std::wcin >> line;
                boost::trim(line);

                if (line.empty() || boost::starts_with(line, L"q")) {
                    quit = true;
                    break;
                }
                if (boost::starts_with(line, L"p")) {
                    start = std::max((int32_t)0, start - hitsPerPage);
                    break;
                } else if (boost::starts_with(line, L"n")) {
                    if (start + hitsPerPage < numTotalHits) {
                        start += hitsPerPage;
                    }
                    break;
                } else {
                    int32_t page = 0;
                    try {
                        page = Lucene::StringUtils::toInt(line);
                    } catch (Lucene::NumberFormatException&) {
                    }
                    if ((page - 1) * hitsPerPage < numTotalHits) {
                        start = std::max((int32_t)0, (page - 1) * hitsPerPage);
                        break;
                    } else {
                        std::wcout << L"No such page\n";
                    }
                }
            }
            if (quit) {
                break;
            }
            end = std::min(numTotalHits, start + hitsPerPage);
        }
    }
}

class StreamingHitCollector : public Lucene::Collector {
public:
    StreamingHitCollector() {
        docBase = 0;
    }

    virtual ~StreamingHitCollector() {
    }

protected:
    Lucene::ScorerPtr scorer;
    int32_t docBase;

public:
    /// simply print docId and score of every matching document
    virtual void collect(int32_t doc) {
        std::wcout << L"doc=" << (doc + docBase) << L" score=" << scorer->score();
    }

    virtual bool acceptsDocsOutOfOrder() {
        return true;
    }

    virtual void setNextReader(const Lucene::IndexReaderPtr& reader, int32_t docBase) {
        this->docBase = docBase;
    }

    virtual void setScorer(const Lucene::ScorerPtr& scorer) {
        this->scorer = scorer;
    }
};

/// This method uses a custom HitCollector implementation which simply prints out the docId and score of
/// every matching document.
///
/// This simulates the streaming search use case, where all hits are supposed to be processed, regardless
/// of their relevance.
static void doStreamingSearch(const Lucene::SearcherPtr& searcher, const Lucene::QueryPtr& query) {
    searcher->search(query, Lucene::newLucene<StreamingHitCollector>());
}

/// Simple command-line based search demo.
int main(int argc, char* argv[]) {
    if (argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
        std::wcout << L"Usage: searchfiles.exe [-index dir] [-field f] [-queries file] [-raw] ";
        std::wcout << L"[-norms field] [-paging hitsPerPage]\n\n";
        std::wcout << L"Specify 'false' for hitsPerPage to use streaming instead of paging search.\n";
        return 1;
    }

    try {
        Lucene::String index = L"index";
        Lucene::String field = SynoTerms::searchFileName;
        Lucene::String queries;
        bool raw = false;
        Lucene::String normsField;
        bool paging = true;
        int32_t hitsPerPage = 10;

        for (int32_t i = 0; i < argc; ++i) {
            if (strcmp(argv[i], "-index") == 0) {
                index = Lucene::StringUtils::toUnicode(argv[i + 1]);
                ++i;
            } else if (strcmp(argv[i], "-field") == 0) {
                field = Lucene::StringUtils::toUnicode(argv[i + 1]);
                ++i;
            } else if (strcmp(argv[i], "-queries") == 0) {
                queries = Lucene::StringUtils::toUnicode(argv[i + 1]);
                ++i;
            } else if (strcmp(argv[i], "-raw") == 0) {
                raw = true;
            } else if (strcmp(argv[i], "-norms") == 0) {
                normsField = Lucene::StringUtils::toUnicode(argv[i + 1]);
                ++i;
            } else if (strcmp(argv[i], "-paging") == 0) {
                if (strcmp(argv[i + 1], "false") == 0) {
                    paging = false;
                } else {
                    hitsPerPage = Lucene::StringUtils::toInt(Lucene::StringUtils::toUnicode(argv[i + 1]));
                    if (hitsPerPage == 0) {
                        paging = false;
                    }
                }
                ++i;
            }
        }

        // RO as only searching
        Lucene::IndexReaderPtr reader = Lucene::IndexReader::open(Lucene::FSDirectory::open(index), true);

        if (!normsField.empty()) {
            reader = Lucene::newLucene<OneNormsReader>(reader, normsField);
        }

        Lucene::SearcherPtr searcher = Lucene::newLucene<Lucene::IndexSearcher>(reader);
        Lucene::AnalyzerPtr analyzer = Lucene::newLucene<Lucene::StandardAnalyzer>(Lucene::LuceneVersion::LUCENE_CURRENT);
        Lucene::QueryParserPtr parser = Lucene::newLucene<Lucene::QueryParser>(Lucene::LuceneVersion::LUCENE_CURRENT, field, analyzer);

        Lucene::ReaderPtr in;
        if (!queries.empty()) {
            in = Lucene::newLucene<Lucene::FileReader>(queries);
        }

        while (true) {
            Lucene::String line;

            if (!queries.empty()) {
                wchar_t c = in->read();
                while (c != L'\n' && c != L'\r' && c != Lucene::Reader::READER_EOF) {
                    line += c;
                    c = in->read();
                }
            } else {
                std::wcout << L"Enter query: ";
                getline(std::wcin, line);
            }
            boost::trim(line);

            if (line.empty()) {
                break;
            }

            Lucene::QueryPtr query = parser->parse(line);
            std::wcout << L"Searching for: " << query->toString(field) << L"\n";

            if (paging) {
                doPagingSearch(searcher, query, hitsPerPage, raw, queries.empty());
            } else {
                doStreamingSearch(searcher, query);
            }
        }
        reader->close();
    } catch (Lucene::LuceneException& e) {
        std::wcout << L"Exception: " << e.getError() << L"\n";
        return 1;
    }

    return 0;
}

