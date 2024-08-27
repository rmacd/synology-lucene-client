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
#include "SearchResults.h"
#include "crow.h"

static std::string wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

float convertToFloat(const Lucene::NumericValue& value) {
    if (const double* v = boost::get<double>(&value)) {
        return static_cast<float>(*v);
    } else if (const int* v = boost::get<int>(&value)) {
        return static_cast<float>(*v);
    } else if (const long long* v = boost::get<long long>(&value)) {
        return static_cast<float>(*v);
    } else {
        throw std::runtime_error("Unsupported numeric type in NumericValue");
    }
}

static Search::SearchResults doSearch(const Lucene::SearcherPtr& searcher, const Lucene::QueryPtr& query, int32_t hitsPerPage) {
    Lucene::TopScoreDocCollectorPtr collector = Lucene::TopScoreDocCollector::create(5 * hitsPerPage, true);

    CROW_LOG_INFO << "Searching for: " << wstring_to_string(query->toString());
    searcher->search(query, collector);
    Lucene::Collection<Lucene::ScoreDocPtr> hits = collector->topDocs()->scoreDocs;

    int32_t numTotalHits = collector->getTotalHits();
    CROW_LOG_INFO << numTotalHits << " matching documents";

    Search::SearchResults sr;
    sr.setTotalHits(numTotalHits);

    int32_t start = 0;
    int32_t end = numTotalHits;

    for (int32_t i = start; i < end; ++i) {
        Search::SearchResult result;

        Lucene::DocumentPtr doc = searcher->doc(hits[i]->doc);
        Lucene::NumericValue score = hits[i]->score;
        result.setScore(convertToFloat(score));

        Lucene::String path = doc->get(SynoTerms::path);
        if (!path.empty()) {
            result.setPath(path);
        }

        Lucene::String title = doc->get(SynoTerms::title);
        if (!title.empty()) {
            result.setTitle(title);
        }

        Lucene::String fsSize = doc->get(SynoTerms::fsSize);
        if (!fsSize.empty()) {
            result.setFsSize(Lucene::StringUtils::toInt(fsSize));
        }

        sr.appendResult(result);
    }

    return sr;
}

Search::SearchResults doSearch(const std::wstring& index, const std::wstring& queryString, const int& hitsPerPage) {

    Lucene::String field = SynoTerms::textContent;

    Lucene::String luceneQuery(queryString);
    Lucene::AnalyzerPtr synoAnalyzer = Lucene::newLucene<Search::SynoAnalyzer>(Lucene::LuceneVersion::LUCENE_29);
    Lucene::QueryParserPtr synoParser = Lucene::newLucene<Lucene::QueryParser>(Lucene::LuceneVersion::LUCENE_29, field, synoAnalyzer);

    Lucene::QueryPtr query = synoParser->parse(luceneQuery);

    // RO as only searching
    Lucene::IndexReaderPtr reader = Lucene::IndexReader::open(Lucene::FSDirectory::open(index), true);
    Lucene::SearcherPtr searcher = Lucene::newLucene<Lucene::IndexSearcher>(reader);

    Search::SearchResults results = doSearch(searcher, query, hitsPerPage);

    if (reader) {
        reader->close();
    }
    return results;
}

crow::json::wvalue to_json(const Search::SearchResult& sr) {
    crow::json::wvalue json;
    if (!sr.getPath().empty()) {
        json["path"] = wstring_to_string(sr.getPath());
    }
    if (!sr.getTitle().empty()) {
        json["title"] = wstring_to_string(sr.getTitle());
    }
    if (sr.getFsSize()) {
        json["fs_size"] = sr.getFsSize();
    }
    if (sr.getScore()) {
        json["score"] = sr.getScore();
    }
    return json;
}

crow::json::wvalue to_json(const Search::SearchResults& sr) {
    crow::json::wvalue json;
    json["total_hits"] = sr.getTotalHits();
    std::vector<crow::json::wvalue> hits;

    if (sr.getTotalHits() > 0) {
        for (const auto& result : sr.getResults()) {
             hits.push_back(to_json(result));
        }
        json["hits"] = std::move(hits);
    }

    return json;
}

int main(int argc, char* argv[]) {

    Lucene::String index = L"index";
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

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return crow::status::NO_CONTENT;
    });

    CROW_ROUTE(app, "/search")([&index, &hitsPerPage](const crow::request& req){
        std::string httpQuery = req.url_params.get("q");
        std::wstring query(httpQuery.begin(), httpQuery.end());

        Search::SearchResults sr = doSearch(index, query, hitsPerPage);
        crow::json::wvalue x = to_json(sr);

        return x;
    });

    app.port(18080).run();

    return 0;
}

