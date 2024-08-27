#include "SynoAnalyzer.h"

#include "lucene++/Lucene.h"
#include "lucene++/LuceneTypes.h"
#include "lucene++/StandardTokenizer.h"
#include "lucene++/StandardFilter.h"
#include "lucene++/LowerCaseFilter.h"
#include "lucene++/PorterStemFilter.h"
#include "lucene++/StopFilter.h"
#include "lucene++/Synchronize.h"

namespace Search {

// https://raw.githubusercontent.com/apache/lucene/main/lucene/analysis/common/src/resources/org/apache/lucene/analysis/cjk/stopwords.txt
const wchar_t* SynoAnalyzer::_ENGLISH_STOP_WORDS[] = {
        L"a", L"and", L"are", L"as", L"at", L"be", L"but",
        L"by", L"for", L"if", L"in", L"into", L"is", L"it",
        L"no", L"not", L"of", L"on", L"or", L"s", L"such",
        L"t", L"that", L"the", L"their", L"then", L"there",
        L"these", L"they", L"this", L"to", L"was", L"will", L"with", L"www"
};

SynoAnalyzer::SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion) {
    this->stopSet = getDefaultStopSet();
    this->matchVersion = matchVersion;
}

SynoAnalyzer::SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion, Lucene::HashSet<Lucene::String> stopwords) {
    this->stopSet = stopwords;
    this->matchVersion = matchVersion;
}

SynoAnalyzer::SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion, Lucene::HashSet<Lucene::String> stopwords, Lucene::HashSet<Lucene::String> exclusions) {
    this->stopSet = stopwords;
    this->exclusionSet = exclusions;
    this->matchVersion = matchVersion;
}

SynoAnalyzer::~SynoAnalyzer() {
}

const Lucene::HashSet<Lucene::String> SynoAnalyzer::getDefaultStopSet() {
    static Lucene::HashSet<Lucene::String> stopSet;
    LUCENE_RUN_ONCE( stopSet = Lucene::HashSet<Lucene::String>::newInstance(_ENGLISH_STOP_WORDS, _ENGLISH_STOP_WORDS + SIZEOF_ARRAY(_ENGLISH_STOP_WORDS)) );
    return stopSet;
}

void SynoAnalyzer::setStemExclusionTable(Lucene::HashSet<Lucene::String> exclusions) {
    exclusionSet = exclusions;
    setPreviousTokenStream(Lucene::LuceneObjectPtr()); // force a new stemmer to be created
}

Lucene::TokenStreamPtr SynoAnalyzer::tokenStream(const Lucene::String& fieldName, const Lucene::ReaderPtr& reader) {
    Lucene::TokenStreamPtr result = Lucene::newLucene<Lucene::StandardTokenizer>(matchVersion, reader);
    result = Lucene::newLucene<Lucene::StandardFilter>(result);
    result = Lucene::newLucene<Lucene::LowerCaseFilter>(result);
    result = Lucene::newLucene<Lucene::StopFilter>(Lucene::StopFilter::getEnablePositionIncrementsVersionDefault(matchVersion), result, stopSet);
    result = Lucene::newLucene<Lucene::PorterStemFilter>(result);
    return result;
}

SynoAnalyzerSavedStreams::~SynoAnalyzerSavedStreams() {
}

}
