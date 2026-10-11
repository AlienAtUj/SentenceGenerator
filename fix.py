import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix pattern selection in generateSentence
old_pattern = """    auto best_pattern = max_element(patterns.begin(), patterns.end(),
        [](const auto& a, const auto& b) { return a.second.confidence < b.second.confidence; });

    const EnhancedPattern& pattern = best_pattern->second;"""

new_pattern = """    vector<pair<string, EnhancedPattern>> sorted_patterns;
    for (const auto& p : patterns) sorted_patterns.push_back(p);
    sort(sorted_patterns.begin(), sorted_patterns.end(),
        [](const auto& a, const auto& b) { return a.second.confidence > b.second.confidence; });

    size_t top_n = min((size_t)5, sorted_patterns.size());
    int choice = rand() % top_n;
    const EnhancedPattern& pattern = sorted_patterns[choice].second;"""

code = code.replace(old_pattern, new_pattern)


# Fix the article bug
old_article = """            // Apply English polish rules
            if (pos == "VERB" && !last_subject.empty()) {
                word = conjugateVerb(word, last_subject);
            }
            else if (pos == "DET" && i + 1 < pattern.posSeq.size()) {
                last_article = word;
                continue;
            }
            else if (!last_article.empty()) {
                word = correctArticle(last_article, word);
                last_article.clear();
            }

            // === ADD THIS NEW LINE ===
            // Apply pronoun case correction
            if (pos == "PRON" && semantic_role == "S") {
                word = correctPronounCase(word, semantic_role);
            }

            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0)) {
                last_subject = word;
            }

            words.push_back(word);"""

new_article = """            // Apply English polish rules
            if (pos == "VERB" && !last_subject.empty()) {
                word = conjugateVerb(word, last_subject);
            }

            if (!last_article.empty()) {
                words.push_back(correctArticle(last_article, word));
                last_article.clear();
            }

            if (pos == "DET" && i + 1 < pattern.posSeq.size()) {
                last_article = word;
                continue;
            }

            // === ADD THIS NEW LINE ===
            // Apply pronoun case correction
            if (pos == "PRON" && semantic_role == "S") {
                word = correctPronounCase(word, semantic_role);
            }

            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0)) {
                last_subject = word;
            }

            words.push_back(word);"""

code = code.replace(old_article, new_article)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
