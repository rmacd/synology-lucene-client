#ifndef SEARCHFILES_SYNOANALYZER_H
#define SEARCHFILES_SYNOANALYZER_H

#include "lucene++/LuceneContrib.h"
#include "lucene++/Analyzer.h"

// is based on the German analyzer

namespace Search {

class LPPCONTRIBAPI SynoAnalyzer : public Lucene::Analyzer {
    public:
    /// Builds an analyzer with the default stop words: {@link #getDefaultStopSet}.
    SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion);

    /// Builds an analyzer with the given stop words.
    SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion, Lucene::HashSet<Lucene::String> stopwords);

    /// Builds an analyzer with the given stop words and stemming exclusion words.
    SynoAnalyzer(Lucene::LuceneVersion::Version matchVersion, Lucene::HashSet<Lucene::String> stopwords, Lucene::HashSet<Lucene::String> exclusions);

    virtual ~SynoAnalyzer();

    protected:
    // stopwords used with the {@link StopFilter}.
    Lucene::HashSet<Lucene::String> stopSet;

    // words that should be indexed but not stemmed.
    Lucene::HashSet<Lucene::String> exclusionSet;

    Lucene::LuceneVersion::Version matchVersion;

    // list of stop words
    static const wchar_t* _ENGLISH_STOP_WORDS[];

    public:
    // unmodifiable instance of the default stop-words set
    static const Lucene::HashSet<Lucene::String> getDefaultStopSet();

    void setStemExclusionTable(Lucene::HashSet<Lucene::String> exclusions);

    /// Creates a {@link TokenStream} which tokenizes all the text in the provided {@link Reader}.
    ///
    /// @return A {@link TokenStream} built from a {@link StandardTokenizer} filtered with
    /// {@link LowerCaseFilter}, {@link StandardFilter}, {@link StopFilter}, and {@link GermanStemFilter}.
    virtual Lucene::TokenStreamPtr tokenStream(const Lucene::String& fieldName, const Lucene::ReaderPtr& reader);

};

class LPPCONTRIBAPI SynoAnalyzerSavedStreams : public Lucene::LuceneObject {
public:
virtual ~SynoAnalyzerSavedStreams();

public:
    Lucene::TokenizerPtr source;
    Lucene::TokenStreamPtr result;
};

}

#endif //SEARCHFILES_SYNOANALYZER_H
