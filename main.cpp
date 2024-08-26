#define CROW_USE_BOOST

#include "lucene++/targetver.h"
#include <iostream>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include "lucene++/LuceneHeaders.h"
#include "lucene++/FilterIndexReader.h"
#include "lucene++/MiscUtils.h"
#include "SynoTerms.h"
#include "SynoAnalyzer.h"
#include "crow.h"

static void doPagingSearch(const Lucene::SearcherPtr& searcher, const Lucene::QueryPtr& query, int32_t hitsPerPage) {
    // Collect enough docs to show 5 pages
    Lucene::TopScoreDocCollectorPtr collector = Lucene::TopScoreDocCollector::create(5 * hitsPerPage, true);
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
//            if (raw) { // output raw format
//                std::wcout << L"doc=" << hits[i]->doc << L" score=" << hits[i]->score << L"\n";
//                continue;
//            }

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

static std::string wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

int main(int argc, char* argv[]) {

    Lucene::String index = L"index";
    Lucene::String field = L"SYNOMDTextContent"; // SynoTerms::textContent;
    Lucene::String queries;
    bool raw = false;
    bool paging = true;
    int32_t hitsPerPage = 10;

    for (int32_t i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-index") == 0) {
            index = Lucene::StringUtils::toUnicode(argv[i + 1]);
            ++i;
        }
    }

    // RO as only searching
    Lucene::IndexReaderPtr reader = Lucene::IndexReader::open(Lucene::FSDirectory::open(index), true);

    Lucene::SearcherPtr searcher = Lucene::newLucene<Lucene::IndexSearcher>(reader);
    Lucene::AnalyzerPtr synoAnalyzer = Lucene::newLucene<Search::SynoAnalyzer>(Lucene::LuceneVersion::LUCENE_29);
    Lucene::QueryParserPtr synoParser = Lucene::newLucene<Lucene::QueryParser>(Lucene::LuceneVersion::LUCENE_29, field, synoAnalyzer);


    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return crow::status::NO_CONTENT;
    });

    CROW_ROUTE(app, "/search")([&synoParser, &hitsPerPage](const crow::request& req){
        std::string httpQuery = req.url_params.get("q");
        std::wstring wideQuery(httpQuery.begin(), httpQuery.end());
        Lucene::String luceneQuery(wideQuery);
        Lucene::QueryPtr luceneQueryPtr = synoParser->parse(luceneQuery);

        std::wcout << L"Searching for: " << luceneQueryPtr->toString() << L"\n";

        crow::json::wvalue x;
        x["pageSize"] = hitsPerPage;
        x["query"] = wstring_to_string(luceneQueryPtr->toString());



        return x;

//        return narrowResult;
    });


    app.port(18080).run();



//    while (true) {
//        Lucene::String line;
//
//        std::wcout << L"Enter query: ";
//        getline(std::wcin, line);
//
//        boost::trim(line);
//
//        if (line.empty()) {
//            break;
//        }
//
//        Lucene::QueryPtr query = synoParser->parse(line);
//        std::wcout << L"Searching for: " << query->toString() << L"\n";
//
//        if (paging) {
//            doPagingSearch(searcher, query, hitsPerPage, raw, queries.empty());
//        } else {
//            doStreamingSearch(searcher, query);
//        }
//    }
    reader->close();

    return 0;
}

