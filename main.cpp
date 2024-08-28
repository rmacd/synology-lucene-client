#define CROW_USE_BOOST

#include "lucene++/targetver.h"
#include <iostream>
#include <codecvt>
#include <cstdint>
#include <boost/algorithm/string.hpp>
#include "lucene++/LuceneHeaders.h"
#include "lucene++/FilterIndexReader.h"

// highlighting
#include "lucene++/Highlighter.h"
#include "lucene++/LuceneHeaders.h"
#include "lucene++/QueryScorer.h"
#include "lucene++/SimpleHTMLFormatter.h"
#include "lucene++/SimpleSpanFragmenter.h"
#include "lucene++/SimpleFragmenter.h"

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

std::vector<std::wstring> convertToVectorStrings(const Lucene::Collection<Lucene::String>& input) {
    std::vector<std::wstring> result;
    for (size_t i = 0; i < input.size(); ++i) {
        Lucene::String luceneStr = input[i];
        result.push_back(luceneStr);
    }
    return result;
}

static Search::SearchResults doSearch(const Lucene::SearcherPtr& searcher, const Lucene::QueryPtr& query, uint32_t hitsPerPage) {

    // maxHits = hitsPerPage * 5
    uint32_t maxHits = 50;
    Lucene::TopScoreDocCollectorPtr collector = Lucene::TopScoreDocCollector::create(maxHits, true);

    CROW_LOG_INFO << "Searching for: " << wstring_to_string(query->toString());
    searcher->search(query, collector);
    Lucene::Collection<Lucene::ScoreDocPtr> hits = collector->topDocs()->scoreDocs;

    uint32_t numTotalHits = collector->getTotalHits();
    CROW_LOG_INFO << numTotalHits << " matching documents";

    Search::SearchResults sr;
    sr.setTotalHits(numTotalHits);

    uint32_t start = 0;
    uint32_t end = (numTotalHits < maxHits) ? numTotalHits : maxHits;

    // set up highlighter etc
    auto analyzer = Lucene::newLucene<Search::SynoAnalyzer>(Lucene::LuceneVersion::LUCENE_CURRENT);
    auto formatter = Lucene::newLucene<Lucene::SimpleHTMLFormatter>(L"<mark>", L"</mark>");
    auto scorer = Lucene::newLucene<Lucene::QueryScorer>(query);
    auto highlighter = Lucene::newLucene<Lucene::Highlighter>(formatter, scorer);
    highlighter->setTextFragmenter(Lucene::newLucene<Lucene::SimpleFragmenter>(50));

    for (uint32_t i = start; i < end; ++i) {
        Search::SearchResult result;

        // grab doc
        Lucene::DocumentPtr doc = searcher->doc(hits[i]->doc);

        // get highlights
        Lucene::String text = doc->get(SynoTerms::textContent);
        Lucene::TokenStreamPtr tokenStream = analyzer->tokenStream(SynoTerms::textContent, Lucene::newLucene<Lucene::StringReader>(text));
        Lucene::Collection<Lucene::String> highlights = highlighter->getBestFragments(tokenStream, text, 3);
        if (!highlights.empty()) {
            result.setHighlights(convertToVectorStrings(highlights));
        }

        // set score
        Lucene::NumericValue score = hits[i]->score;
        result.setScore(convertToFloat(score));

        // path
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

        Lucene::String extension = doc->get(SynoTerms::extension);
        if (!extension.empty()) {
            result.setExtension(extension);
        }

        Lucene::String explanation = searcher->explain(query, hits[i]->doc)->toHtml();

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
    if (!sr.getExtension().empty()) {
        json["extension"] = wstring_to_string(sr.getExtension());
    }
    if (!sr.getHighlights().empty()) {
        std::vector<crow::json::wvalue> highlights;
        for (const auto& highlight : sr.getHighlights()) {
            highlights.push_back(wstring_to_string(highlight));
        }
        json["highlights"] = std::move(highlights);
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
    uint32_t hitsPerPage = 10;

    for (uint32_t i = 0; i < argc; ++i) {
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

    CROW_ROUTE(app, "/get")([](const crow::request& req, crow::response& res){
        std::string path = req.url_params.get("p");
        std::ifstream file_stream(path, std::ios::binary);

        if (!file_stream) {
            res.code = 404;
            res.end("File not found");
            return;
        }

        file_stream.seekg(0, std::ios::end);
        std::streamsize size = file_stream.tellg();
        file_stream.seekg(0, std::ios::beg);

        // Read the file content into a buffer
        std::vector<char> buffer(size);
        if (file_stream.read(buffer.data(), size)) {
            if (path.find(".pdf") != std::string::npos) {
                res.set_header("Content-Type", "application/pdf");
            } else if (path.find(".docx") != std::string::npos) {
                res.set_header("Content-Type", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
            } else {
                res.set_header("Content-Type", "application/octet-stream");
            }

            // Encourage the browser to display the file inline
            res.set_header("Content-Disposition", "inline; filename=" + path);
            res.set_header("Content-Length", std::to_string(buffer.size()));
            res.body.assign(buffer.begin(), buffer.end());
            res.code = 200;
        } else {
            res.code = 500;
            res.end("Failed to read the file");
        }

        res.end();
    });

    app.port(18080).run();

    return 0;
}

