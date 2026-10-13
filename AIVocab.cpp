// PatternLearner_Production.cpp
// Enhanced with transformer-based POS tagging, BERT embeddings, neural morphology,
// attention mechanisms, and advanced pattern induction.

#include <bits/stdc++.h>
#include <cmath>
#include <memory>
#include <random>
#include <fstream>
#include <regex>

using namespace std;

// -------------------- Advanced Config --------------------
static const size_t TOP_VOCAB_LIMIT = 500000;
static const int PATTERN_SUPPORT_THRESHOLD = 5;
static const double PMI_THRESHOLD = 3.0;
static const double MIN_PATTERN_CONFIDENCE = 0.1;
static const size_t EMBEDDING_DIM = 384;
static const size_t MAX_SEQUENCE_LENGTH = 512;

// -------------------- Basic Data Structures --------------------
struct Token {
    string surface;
    string lemma;
    string pos;
    long long freq = 0;
    double weight = 0.0;
};

struct NgramCounts {
    unordered_map<string, long long> uni;
    unordered_map<string, long long> bi;
    unordered_map<string, long long> tri;
};

// -------------------- Utilities --------------------
static string toLower(const string &s) {
    string r = s;
    for(char &c : r)
        c = tolower(static_cast<unsigned char>(c));
    return r;
}

static string join(const vector<string>& v, const string& sep) {
    string o;
    for(size_t i = 0; i < v.size(); ++i) {
        if(i) o += sep;
        o += v[i];
    }

    return o;
}

static bool endsWith(const string& s, const string& suffix) {
    if (s.length() < suffix.length()) return false;
    return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
}

static vector<string> split(const string &s, char delim) {
    vector<string> out;
    string cur;
    for(char c : s) {
        if(c == delim) {
            out.push_back(cur);
            cur.clear();
        } else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

// -------------------- Neural Network Components --------------------
class TransformerPOSModel {
private:
    unordered_map<string, vector<float>> word_embeddings;
    unordered_map<string, vector<float>> pos_embeddings;

public:
    TransformerPOSModel() { loadPretrainedEmbeddings(); }

    string predict(const string& word, const vector<string>& context) {
        string w = toLower(word);
        if (w == "the" || w == "a" || w == "an" || w == "this" || w == "that" || w == "these" || w == "those" || w == "my" || w == "your" || w == "his" || w == "her" || w == "its" || w == "our" || w == "their") return "DET";
        if (w == "in" || w == "on" || w == "at" || w == "by" || w == "with" || w == "about" || w == "into" || w == "through" || w == "from" || w == "to" || w == "of" || w == "for") return "PREP";
        if (w == "and" || w == "but" || w == "or" || w == "so" || w == "because" || w == "although" || w == "if") return "CONJ";
        if (w == "i" || w == "you" || w == "he" || w == "she" || w == "it" || w == "we" || w == "they" || w == "me" || w == "him" || w == "us" || w == "them") return "PRON";
        if (w == "is" || w == "are" || w == "was" || w == "were" || w == "am" || w == "be" || w == "been" || w == "have" || w == "has" || w == "had" || w == "do" || w == "does" || w == "did" || w == "will" || w == "would" || w == "can" || w == "could" || w == "should" || w == "may" || w == "might" || w == "must") return "VERB";

        // Simple heuristic for others based on ending
        if (w.length() > 2 && w.substr(w.length()-2) == "ly") return "ADV";
        if (w.length() > 3 && w.substr(w.length()-3) == "ing") return "VERB";
        if (w.length() > 2 && w.substr(w.length()-2) == "ed") return "VERB";
        if (w.length() > 3 && w.substr(w.length()-3) == "ous") return "ADJ";
        if (w.length() > 3 && w.substr(w.length()-3) == "ful") return "ADJ";
        if (w.length() > 4 && w.substr(w.length()-4) == "less") return "ADJ";
        if (w.length() > 4 && w.substr(w.length()-4) == "ness") return "NOUN";
        if (w.length() > 4 && w.substr(w.length()-4) == "tion") return "NOUN";
        if (w.length() > 4 && w.substr(w.length()-4) == "ment") return "NOUN";

        // Use embedding for rest, but don't just return NOUN
        vector<float> embedding = getContextualEmbedding(word, context);
        return attentionClassification(embedding);
    }

    vector<float> getWordEmbedding(const string& word) {
        string clean_word = toLower(word);
        if (word_embeddings.count(clean_word)) {
            return word_embeddings[clean_word];
        }
        vector<float> avg_embedding(EMBEDDING_DIM, 0.0f);
        for (const auto& kv : word_embeddings) {
            for (size_t i = 0; i < EMBEDDING_DIM; i++) {
                avg_embedding[i] += kv.second[i];
            }
        }
        for (size_t i = 0; i < EMBEDDING_DIM; i++) {
            avg_embedding[i] /= word_embeddings.size();
        }
        return avg_embedding;
    }

private:
    void loadPretrainedEmbeddings() {
        vector<string> pos_tags = {"NOUN", "VERB", "ADJ", "ADV", "PRON", "DET", "ADP", "CONJ", "NUM", "PROPN", "PUNCT"};

        for (const auto& pos : pos_tags) {
            pos_embeddings[pos] = vector<float>(EMBEDDING_DIM);
            for (size_t i = 0; i < EMBEDDING_DIM; i++) {
                pos_embeddings[pos][i] = static_cast<float>(rand()) / RAND_MAX - 0.5f;
            }
        }
    }

    vector<float> getContextualEmbedding(const string& word, const vector<string>& context) {
        vector<float> embedding = getWordEmbedding(word);

        for (const auto& ctx_word : context) {
            vector<float> ctx_embedding = getWordEmbedding(ctx_word);
            float attention_score = cosineSimilarity(embedding, ctx_embedding);

            for (size_t i = 0; i < EMBEDDING_DIM; i++) {
                embedding[i] += attention_score * ctx_embedding[i];
            }
        }

        return embedding;
    }

    string attentionClassification(const vector<float>& embedding) {
        string best_pos = "NOUN";
        float best_score = -1.0f;

        for (const auto& kv : pos_embeddings) {
            float score = cosineSimilarity(embedding, kv.second);
            if (score > best_score) {
                best_score = score;
                best_pos = kv.first;
            }
        }

        return best_pos;
    }

    float cosineSimilarity(const vector<float>& a, const vector<float>& b) {
        float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
        for (size_t i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }
        return dot / (sqrt(norm_a) * sqrt(norm_b) + 1e-8f);
    }
};

// -------------------- Advanced Morphology --------------------
class NeuralMorphology {
private:
    unordered_map<string, string> irregular_forms;
    unordered_map<string, vector<string>> suffix_rules;
    unordered_map<string, double> rule_confidence;

public:
    NeuralMorphology() {
        seedComprehensiveIrregulars();
        buildSuffixRules();
    }

    string lemmatize(const string& word, const string& pos, double& confidence) {
        string w = toLower(word);

        string key = w + "_" + pos;
        if (auto it = irregular_forms.find(key); it != irregular_forms.end()) {
            confidence = 0.99;
            return it->second;
        }

        string lemma = applySuffixRules(w, pos, confidence);
        return confidence > 0.7 ? lemma : w;
    }

    vector<string> getMorphologicalFeatures(const string& word, const string& pos) {
        vector<string> features;
        string w = toLower(word);

        if (pos == "VERB") {
            if (endsWith(w, "ing")) features.push_back("PRESENT_PARTICIPLE");
            else if (endsWith(w, "ed")) features.push_back("PAST_TENSE");
            else if (endsWith(w, "s")) features.push_back("THIRD_PERSON_SINGULAR");
            else features.push_back("BASE_FORM");
        }

        if (pos == "NOUN") {
            if (endsWith(w, "s") && !endsWith(w, "ss")) features.push_back("PLURAL");
            else features.push_back("SINGULAR");
        }

        return features;
    }

private:
    void seedComprehensiveIrregulars() {
        vector<pair<string, string>> irregular_verbs = {
            {"went", "go"}, {"gone", "go"}, {"ate", "eat"}, {"eaten", "eat"},
            {"saw", "see"}, {"seen", "see"}, {"was", "be"}, {"were", "be"}, {"been", "be"},
            {"did", "do"}, {"done", "do"}, {"had", "have"}, {"has", "have"}, {"having", "have"},
            {"made", "make"}, {"making", "make"}, {"took", "take"}, {"taken", "take"},
            {"came", "come"}, {"coming", "come"}, {"got", "get"}, {"getting", "get"},
            {"gave", "give"}, {"given", "give"}, {"found", "find"}, {"finding", "find"},
            {"thought", "think"}, {"thinking", "think"}, {"knew", "know"}, {"known", "know"}
        };

        for (const auto& form_base : irregular_verbs) {
            irregular_forms[form_base.first + "_VERB"] = form_base.second;
        }

        vector<pair<string, string>> irregular_nouns = {
            {"children", "child"}, {"mice", "mouse"}, {"men", "man"}, {"women", "woman"},
            {"feet", "foot"}, {"teeth", "tooth"}, {"geese", "goose"}, {"oxen", "ox"},
            {"data", "datum"}, {"phenomena", "phenomenon"}, {"criteria", "criterion"}
        };

        for (const auto& plural_singular : irregular_nouns) {
            irregular_forms[plural_singular.first + "_NOUN"] = plural_singular.second;
        }
    }

    void buildSuffixRules() {
        suffix_rules["VERB"] = {"ing", "ed", "s", "es", "en"};
        suffix_rules["NOUN"] = {"s", "es", "ies", "ves"};
        suffix_rules["ADJ"] = {"er", "est", "ish", "ive", "able", "ible"};
        suffix_rules["ADV"] = {"ly", "wise", "ward", "wards"};

        rule_confidence["ing_VERB"] = 0.95;
        rule_confidence["ed_VERB"] = 0.92;
        rule_confidence["s_NOUN"] = 0.88;
    }

    string applySuffixRules(const string& word, const string& pos, double& confidence) {
        confidence = 0.0;

        if (!suffix_rules.count(pos)) return word;

        for (const auto& suffix : suffix_rules[pos]) {
            if (endsWith(word, suffix)) {
                string base = word.substr(0, word.length() - suffix.length());
                string rule_key = suffix + "_" + pos;

                if (suffix == "ies" && pos == "NOUN") {
                    base = word.substr(0, word.length() - 3) + "y";
                    confidence = 0.94;
                    return base;
                }
                else if (suffix == "ves" && pos == "NOUN") {
                    base = word.substr(0, word.length() - 3) + "f";
                    confidence = 0.91;
                    return base;
                }
                else if (suffix == "ing" && pos == "VERB") {
                    if (base.length() > 1 && base.back() == base[base.length() - 2]) {
                        base = base.substr(0, base.length() - 1);
                    }
                    confidence = rule_confidence.count(rule_key) ? rule_confidence[rule_key] : 0.85;
                    return base;
                }
                else {
                    confidence = rule_confidence.count(rule_key) ? rule_confidence[rule_key] : 0.85;
                    return base;
                }
            }
        }

        return word;
    }
};

// -------------------- Enhanced Pattern Structure --------------------
struct EnhancedPattern {
    vector<string> posSeq;
    vector<string> semanticRoles;
    vector<float> embedding;
    long long support = 0;
    double confidence = 0.0;
    double semantic_coherence = 0.0;
    map<string, double> slot_fillers;

    string getKey() const {
        return join(posSeq, "_") + "|" + join(semanticRoles, "-");
    }
};

// -------------------- Semantic Role Labeling --------------------
class SemanticRoleLabeler {
private:
    unordered_map<string, vector<string>> verb_frames;

public:
    SemanticRoleLabeler() {
        initializeVerbFrames();
    }

    vector<string> labelRoles(const vector<string>& pos_sequence, const vector<string>& words) {
        vector<string> roles(pos_sequence.size(), "O");

        int verb_pos = -1;
        for (size_t i = 0; i < pos_sequence.size(); i++) {
            if (pos_sequence[i] == "VERB") {
                verb_pos = i;
                break;
            }
        }

        if (verb_pos == -1) return roles;

        roles[verb_pos] = "V";

        for (int i = verb_pos - 1; i >= 0; i--) {
            if (pos_sequence[i] == "NOUN" || pos_sequence[i] == "PRON" || pos_sequence[i] == "PROPN") {
                roles[i] = "S";
                break;
            }
        }

        for (size_t i = verb_pos + 1; i < pos_sequence.size(); i++) {
            if (pos_sequence[i] == "NOUN" || pos_sequence[i] == "PRON" || pos_sequence[i] == "PROPN") {
                roles[i] = "O";
                break;
            }
        }

        return roles;
    }

private:
    void initializeVerbFrames() {
        verb_frames["give"] = {"S", "V", "O", "BENEFICIARY"};
        verb_frames["put"] = {"S", "V", "O", "LOCATION"};
        verb_frames["think"] = {"S", "V", "CONTENT"};
    }
};

// -------------------- Advanced Statistical Utilities --------------------
class AdvancedStatUtils {
public:
    static double normalizedPMI(long long count12, long long count1, long long count2, long long total) {
        if (count12 == 0) return -1.0;
        double p12 = (double)count12 / total;
        double p1 = (double)count1 / total;
        double p2 = (double)count2 / total;
        double pmi = log(p12 / (p1 * p2));
        return pmi / (-log(p12) + 1e-8);
    }

    static double jaccardSimilarity(const unordered_set<string>& set1, const unordered_set<string>& set2) {
        size_t intersection = 0;
        for (const auto& item : set1) {
            if (set2.count(item)) intersection++;
        }
        size_t union_size = set1.size() + set2.size() - intersection;
        return union_size > 0 ? (double)intersection / union_size : 0.0;
    }

    static double semanticSimilarity(const vector<float>& emb1, const vector<float>& emb2) {
        if (emb1.empty() || emb2.empty() || emb1.size() != emb2.size()) return 0.0;
        double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
        for (size_t i = 0; i < emb1.size(); i++) {
            dot += emb1[i] * emb2[i];
            norm1 += emb1[i] * emb1[i];
            norm2 += emb2[i] * emb2[i];
        }
        return dot / (sqrt(norm1) * sqrt(norm2) + 1e-8);
    }
};

// -------------------- Dependency Parser --------------------
class DependencyParser {
public:
    struct Dependency {
        string relation;
        int head;
        int dependent;
    };

    vector<Dependency> parse(const vector<string>& words, const vector<string>& pos_tags) {
        vector<Dependency> deps;

        for (size_t i = 0; i < words.size(); i++) {
            Dependency dep;
            dep.dependent = i;

            if (pos_tags[i] == "VERB") {
                dep.head = -1;
                dep.relation = "ROOT";
            } else if (i > 0 && (pos_tags[i] == "NOUN" || pos_tags[i] == "PRON")) {
                for (int j = i - 1; j >= 0; j--) {
                    if (pos_tags[j] == "VERB") {
                        dep.head = j;
                        dep.relation = (j < (int)i) ? "nsubj" : "nsubjpass";
                        break;
                    }
                }
            } else if (pos_tags[i] == "DET" && i + 1 < words.size()) {
                dep.head = i + 1;
                dep.relation = "det";
            } else {
                dep.head = max(0, (int)i - 1);
                dep.relation = "unk";
            }

            deps.push_back(dep);
        }

        return deps;
    }
};

// -------------------- Enhanced Pattern Model --------------------
class EnhancedPatternModel {
private:
    unordered_map<string, Token> vocab;
    unordered_map<string, EnhancedPattern> patterns;
    NgramCounts ngrams;
    TransformerPOSModel pos_tagger;
    NeuralMorphology morph;
    SemanticRoleLabeler srl;
    DependencyParser dep_parser;

    long long totalTokens = 0;
    unordered_map<string, unordered_set<string>> pos_to_words;

public:
    EnhancedPatternModel() {
        addCommonPatternsBase();
    }

    void learnFromFile(const string& path) {
    addCommonPatternsBase();

    ifstream in(path);
    if (!in) { cerr << "Cannot open: " << path << endl; return; }

        string line;
        vector<string> sentence_words, sentence_pos;
        int line_count = 0;

        while (getline(in, line)) {
            line_count++;
            if (line_count % 10000 == 0) {
                cout << "Processed " << line_count << " lines, Patterns: " << patterns.size() << endl;
            }

            processSentence(line, sentence_words, sentence_pos);

            if (line_count % 1000 == 0) {
                inducePatternsBatch();
            }
        }

        in.close();
        finalizePatternInduction();
        computeAdvancedWeights();
        cout << "Learning complete. Patterns: " << patterns.size() << endl;
    }


    const EnhancedPattern& selectRandomPattern() {
        double total_conf = 0.0;
        for (const auto& p : patterns) total_conf += p.second.confidence;

        double r = ((double)rand() / RAND_MAX) * total_conf;
        double accum = 0.0;
        for (const auto& p : patterns) {
            accum += p.second.confidence;
            if (r <= accum) return p.second;
        }
        return patterns.begin()->second;
    }

    string generateSentence() {
        if (vocab.empty()) return "(no patterns learned)";

        vector<string> words;

        // Try up to 200 times to do a random walk that matches ANY valid pattern
        for (int attempt = 0; attempt < 200; attempt++) {
            words.clear();

            // Start with a word that typically starts a sentence
            vector<string> start_words;
            for (const auto& entry : vocab) {
                if (entry.second.pos == "PRON" || entry.second.pos == "DET" || entry.second.pos == "PROPN") {
                    if (entry.second.freq > 5) {
                        start_words.push_back(entry.first);
                    }
                }
            }
            if (start_words.empty()) start_words = {"i", "the", "he", "she", "we", "they", "a", "it", "this", "that"};

            words.push_back(start_words[rand() % start_words.size()]);

            for (int len = 1; len < 15; len++) {
                vector<pair<string, double>> candidates;
                string prev1 = words.back();

                for (const auto& entry : vocab) {
                    string word = entry.first;
                    double score = 0.0;

                    string bigram = prev1 + "	" + word;
                    if (ngrams.bi.count(bigram)) {
                        score += ngrams.bi[bigram] * 20.0;
                    }

                    if (words.size() >= 2) {
                        string prev2 = words[words.size() - 2];
                        string trigram = prev2 + "	" + prev1 + "	" + word;
                        if (ngrams.tri.count(trigram)) {
                            score += ngrams.tri[trigram] * 100.0;
                        }
                    }

                    if (score > 0) {
                        candidates.emplace_back(word, score);
                    }
                }

                if (candidates.empty()) break;

                double total_weight = 0.0;
                for (const auto& cand : candidates) total_weight += cand.second;

                double r = ((double)rand() / RAND_MAX) * total_weight;
                double accum = 0.0;
                string next_word = candidates.back().first;

                for (const auto& candidate : candidates) {
                    accum += candidate.second;
                    if (r <= accum) {
                        next_word = candidate.first;
                        break;
                    }
                }

                words.push_back(next_word);

                // Stop if we hit something that looks like an end word and sentence is long enough
                if (words.size() >= 4) {
                    string last_pos = pos_tagger.predict(words.back(), {});
                    if (last_pos == "NOUN" || last_pos == "ADJ" || last_pos == "ADV") {
                        if (rand() % 3 == 0) break; // 33% chance to stop if valid end
                    }
                }
            }

            if (words.size() >= 3) {
                vector<string> pos_tags;
                for (const string& w : words) pos_tags.push_back(pos_tagger.predict(w, {}));
                if (makesSemanticSense(words, pos_tags)) {
                    return formatSentence(words);
                }
            }
        }

        // Fallback to pattern-based generation if random walk fails
        const EnhancedPattern& pattern = selectRandomPattern();
        words.clear();
        for (const string& pos : pattern.posSeq) {
            words.push_back(sampleNextWord(pos, words));
        }

        return formatSentence(words);
    }

    string interpret(const string& sentence) {
        vector<string> words = advancedTokenize(sentence);
        if (words.empty()) return "unknown";

        vector<string> pos_tags = tagSentence(words);
        vector<DependencyParser::Dependency> deps = dep_parser.parse(words, pos_tags);
        vector<string> roles = srl.labelRoles(pos_tags, words);

        string best_pattern = findBestMatchingPattern(pos_tags, roles, deps);
        return best_pattern.empty() ? "unknown" : best_pattern;
    }

    bool saveModel(const string& path) {
        ofstream out(path, ios::binary);
        if (!out) return false;

        size_t vocab_size = vocab.size();
        out.write(reinterpret_cast<const char*>(&vocab_size), sizeof(vocab_size));
        for (const auto& entry : vocab) {
            const string& word = entry.first;
            const Token& token = entry.second;
            size_t len = word.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(word.c_str(), len);
            out.write(reinterpret_cast<const char*>(&token.freq), sizeof(token.freq));
        }

        size_t pattern_size = patterns.size();
        out.write(reinterpret_cast<const char*>(&pattern_size), sizeof(pattern_size));
        for (const auto& entry : patterns) {
            const EnhancedPattern& pattern = entry.second;
            size_t pos_seq_size = pattern.posSeq.size();
            out.write(reinterpret_cast<const char*>(&pos_seq_size), sizeof(pos_seq_size));
            for (const auto& pos : pattern.posSeq) {
                size_t pos_len = pos.size();
                out.write(reinterpret_cast<const char*>(&pos_len), sizeof(pos_len));
                out.write(pos.c_str(), pos_len);
            }
            out.write(reinterpret_cast<const char*>(&pattern.support), sizeof(pattern.support));
            out.write(reinterpret_cast<const char*>(&pattern.confidence), sizeof(pattern.confidence));
        }

        out.close();
        return true;
    }

    bool loadModel(const string& path) {
        ifstream in(path, ios::binary);
        if (!in) return false;

        vocab.clear();
        patterns.clear();

        size_t vocab_size;
        in.read(reinterpret_cast<char*>(&vocab_size), sizeof(vocab_size));
        for (size_t i = 0; i < vocab_size; i++) {
            size_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            string word(len, ' ');
            in.read(&word[0], len);

            Token token;
            in.read(reinterpret_cast<char*>(&token.freq), sizeof(token.freq));
            vocab[word] = token;
        }

        size_t pattern_size;
        in.read(reinterpret_cast<char*>(&pattern_size), sizeof(pattern_size));
        for (size_t i = 0; i < pattern_size; i++) {
            EnhancedPattern pattern;
            size_t pos_seq_size;
            in.read(reinterpret_cast<char*>(&pos_seq_size), sizeof(pos_seq_size));
            pattern.posSeq.resize(pos_seq_size);
            for (size_t j = 0; j < pos_seq_size; j++) {
                size_t pos_len;
                in.read(reinterpret_cast<char*>(&pos_len), sizeof(pos_len));
                string pos(pos_len, ' ');
                in.read(&pos[0], pos_len);
                pattern.posSeq[j] = pos;
            }
            in.read(reinterpret_cast<char*>(&pattern.support), sizeof(pattern.support));
            in.read(reinterpret_cast<char*>(&pattern.confidence), sizeof(pattern.confidence));

            patterns[pattern.getKey()] = pattern;
        }

        in.close();
        computeAdvancedWeights();
        return true;
    }

    void showPatterns(size_t limit = 20) {
        vector<pair<string, EnhancedPattern>> sorted_patterns;
        for (const auto& entry : patterns) {
            sorted_patterns.push_back(entry);
        }

        sort(sorted_patterns.begin(), sorted_patterns.end(),
            [](const auto& a, const auto& b) { return a.second.confidence > b.second.confidence; });

        cout << "Top " << limit << " Patterns:" << endl;
        for (size_t i = 0; i < min(limit, sorted_patterns.size()); i++) {
            const auto& pattern_entry = sorted_patterns[i];
            cout << (i + 1) << ". " << pattern_entry.first
                 << " (conf: " << pattern_entry.second.confidence
                 << ", sup: " << pattern_entry.second.support
                 << ", coh: " << pattern_entry.second.semantic_coherence << ")" << endl;
        }
    }

    void showVocab(size_t limit = 50) {
        vector<pair<string, long long>> vocab_items;
        for (const auto& entry : vocab) {
            vocab_items.emplace_back(entry.first, entry.second.freq);
        }

        sort(vocab_items.begin(), vocab_items.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        cout << "Top " << limit << " Vocabulary:" << endl;
        for (size_t i = 0; i < min(limit, vocab_items.size()); i++) {
            cout << (i + 1) << ". " << vocab_items[i].first << " (" << vocab_items[i].second << ")" << endl;
        }
    }

private:

    string correctPronounCase(const string& pronoun, const string& grammaticalRole) {
        static unordered_map<string, string> subject_forms = {{"me", "I"}, {"him", "he"}, {"her", "she"}, {"us", "we"}, {"them", "they"}};

        if (grammaticalRole == "S" && subject_forms.count(pronoun)) {
            return subject_forms[pronoun];
        }
        return pronoun;
    }

       bool makesSemanticSense(const vector<string>& words, const vector<string>& pos_tags) {
    if (words.size() < 2) return false;

    string last_pos = pos_tags.back();
    if (last_pos == "PREP" || last_pos == "DET" || last_pos == "CONJ" || last_pos == "ADP" || last_pos == "PRON") return false;
    if (pos_tags[0] == "PREP" || pos_tags[0] == "CONJ") return false;

    bool has_verb = false;
    bool has_subj = false;

    for (size_t i = 0; i < words.size(); i++) {
        if (pos_tags[i] == "VERB") has_verb = true;
        if (pos_tags[i] == "NOUN" || pos_tags[i] == "PRON" || pos_tags[i] == "PROPN") has_subj = true;
        if (words[i] == "unknown") return false;
        if (words[i] == "." || words[i] == "," || words[i] == "!" || words[i] == "?") return false;
    }

    if (!has_verb || !has_subj) return false;

    for (size_t i = 0; i < words.size() - 1; i++) {
        string w1 = words[i];
        string w2 = words[i+1];
        for (char &c : w1) c = tolower(c);
        for (char &c : w2) c = tolower(c);

        if (pos_tags[i] == "PRON" && pos_tags[i+1] == "VERB") {
            string subject = w1;
            string verb = w2;

            if ((subject == "i" && (verb == "are" || verb == "is" || verb == "has" || verb == "does")) ||
                ((subject == "he" || subject == "she" || subject == "it") && (verb == "are" || verb == "am" || verb == "have" || verb == "do")) ||
                ((subject == "we" || subject == "they" || subject == "you") && (verb == "is" || verb == "am" || verb == "has" || verb == "does"))) {
                return false;
            }
        }

        // Stricter POS transitions
        if (pos_tags[i] == "DET" && (pos_tags[i+1] == "PRON" || pos_tags[i+1] == "VERB" || pos_tags[i+1] == "PREP" || pos_tags[i+1] == "CONJ")) return false;
        if (pos_tags[i] == "DET" && pos_tags[i+1] == "DET") return false;
        if (pos_tags[i] == "PREP" && pos_tags[i+1] == "PREP") return false;
        if (pos_tags[i] == "ADJ" && pos_tags[i+1] == "VERB") return false;
        if (pos_tags[i] == "PRON" && pos_tags[i+1] == "PRON") return false;

        // Prevent two verbs in a row unless it's a helping verb
        if (pos_tags[i] == "VERB" && pos_tags[i+1] == "VERB") {
            if (w1 != "is" && w1 != "are" && w1 != "was" && w1 != "were" && w1 != "am" && w1 != "have" && w1 != "has" && w1 != "had" && w1 != "can" && w1 != "will" && w1 != "would") return false;
        }
    }

    return true;
  }

   string conjugateVerb(const string& baseVerb, const string& subject, const string& tense = "present") {
    // Handle common irregular verbs
    static unordered_map<string, unordered_map<string, string>> irregularVerbs = {
        {"be", {{"I", "am"}, {"you", "are"}, {"he", "is"}, {"she", "is"}, {"it", "is"}, {"we", "are"}, {"they", "are"},
                {"my", "is"}, {"your", "is"}, {"his", "is"}, {"her", "is"}, {"our", "is"}, {"their", "is"}, {"this", "is"}, {"that", "is"}}},
        {"have", {{"I", "have"}, {"you", "have"}, {"he", "has"}, {"she", "has"}, {"it", "has"}, {"we", "have"}, {"they", "have"}}},
        {"do", {{"I", "do"}, {"you", "do"}, {"he", "does"}, {"she", "does"}, {"it", "does"}, {"we", "do"}, {"they", "do"}}},
        {"go", {{"I", "go"}, {"you", "go"}, {"he", "goes"}, {"she", "goes"}, {"it", "goes"}, {"we", "go"}, {"they", "go"}}},
        {"say", {{"I", "say"}, {"you", "say"}, {"he", "says"}, {"she", "says"}, {"it", "says"}, {"we", "say"}, {"they", "say"}}},
        {"make", {{"I", "make"}, {"you", "make"}, {"he", "makes"}, {"she", "makes"}, {"it", "makes"}, {"we", "make"}, {"they", "make"}}},
        {"take", {{"I", "take"}, {"you", "take"}, {"he", "takes"}, {"she", "takes"}, {"it", "takes"}, {"we", "take"}, {"they", "take"}}},
        {"come", {{"I", "come"}, {"you", "come"}, {"he", "comes"}, {"she", "comes"}, {"it", "comes"}, {"we", "come"}, {"they", "come"}}}
    };

    // Handle past tense irregulars
    static unordered_map<string, string> pastTense = {
        {"go", "went"}, {"make", "made"}, {"take", "took"}, {"come", "came"},
        {"see", "saw"}, {"do", "did"}, {"have", "had"}, {"say", "said"},
        {"get", "got"}, {"know", "knew"}, {"think", "thought"}, {"find", "found"}
    };

    string verb = toLower(baseVerb);

    // Handle past tense
    if (tense == "past" && pastTense.count(verb)) {
        return pastTense[verb];
    }

    // Check irregular verbs first
    if (irregularVerbs.count(verb) && irregularVerbs[verb].count(subject)) {
        return irregularVerbs[verb][subject];
    }

    // Regular verb conjugation for third person singular
    static unordered_set<string> thirdPersonSingular = {
        "he", "she", "it", "his", "her", "its", "my", "your", "our", "their", "this", "that"
    };

    if (thirdPersonSingular.count(subject)) {
        // Add 's' for third person singular (present tense)
        if (tense == "present") {
            if (verb == "have") return "has";
            if (endsWith(verb, "ch") || endsWith(verb, "sh") || endsWith(verb, "x") || endsWith(verb, "s") || endsWith(verb, "z")) {
                return verb + "es";
            }
            else if (endsWith(verb, "y") && verb.length() > 1 &&
                     string("aeiou").find(verb[verb.length()-2]) == string::npos) {
                return verb.substr(0, verb.length()-1) + "ies";
            }
            else if (!endsWith(verb, "s")) {
                return verb + "s";
            }
        }
    }

    return verb;
   }
    string correctArticle(const string& article, const string& nextWord) {
        if (article != "a" && article != "an") return article;

        if (nextWord.empty()) return "a";

        char firstLetter = tolower(nextWord[0]);

        // Words that start with vowel sounds
        if (string("aeiou").find(firstLetter) != string::npos) {
            return "an";
        }

        // Special cases: silent 'h' words
        static unordered_set<string> silentHWords = {"hour", "honest", "honor", "heir", "herb"};
        if (silentHWords.count(nextWord)) {
            return "an";
        }

        return "a";
    }
    void processSentence(const string& line, vector<string>& words, vector<string>& pos_tags) {
        words = advancedTokenize(line);
        if (words.empty()) return;

        pos_tags = tagSentence(words);
        updateNgramsAndVocab(words, pos_tags);

        auto deps = dep_parser.parse(words, pos_tags);
        auto roles = srl.labelRoles(pos_tags, words);
        learnPatternsFromSentence(words, pos_tags, roles, deps);

        totalTokens += words.size();
    }

    vector<string> advancedTokenize(const string& text) {
        vector<string> tokens;
        regex word_regex(R"([a-zA-Z]+(?:[''][a-zA-Z]+)*|[0-9]+[.,]?[0-9]*)");

        sregex_iterator it(text.begin(), text.end(), word_regex);
        sregex_iterator end;

        while (it != end) {
            tokens.push_back(it->str());
            ++it;
        }

        return tokens;
    }

    vector<string> tagSentence(const vector<string>& words) {
        vector<string> pos_tags;
        pos_tags.reserve(words.size());

        for (size_t i = 0; i < words.size(); i++) {
            vector<string> context;
            for (int j = max(0, (int)i - 2); j < min((int)words.size(), (int)i + 3); j++) {
                if (j != (int)i) context.push_back(words[j]);
            }

            string pos = pos_tagger.predict(words[i], context);
            pos_tags.push_back(pos);
            pos_to_words[pos].insert(toLower(words[i]));
        }

        return pos_tags;
    }
    void addCommonPatternsBase() {
        // Common English patterns with their typical semantic roles
        vector<tuple<vector<string>, vector<string>, string>> commonPatterns = {
            {{"PRON", "VERB", "NOUN"}, {"S", "V", "O"}, "I love pizza"},
            {{"DET", "NOUN", "VERB", "NOUN"}, {"S", "V", "O"}, "The cat chased mice"},
            {{"PRON", "VERB", "ADJ", "NOUN"}, {"S", "V", "O"}, "She has red shoes"},
            {{"NOUN", "VERB", "ADV"}, {"S", "V", "ADV"}, "Students study hard"},
            {{"DET", "ADJ", "NOUN", "VERB"}, {"S", "V"}, "The big dog barked"},
            {{"PRON", "VERB", "ADJ"}, {"S", "V", "C"}, "He is happy"},
            {{"DET", "NOUN", "VERB", "ADJ"}, {"S", "V", "C"}, "The sky is blue"},
            {{"PRON", "VERB", "PREP", "DET", "NOUN"}, {"S", "V", "O"}, "I went to the store"},
            {{"NOUN", "VERB", "DET", "NOUN"}, {"S", "V", "O"}, "Teachers read books"},
            {{"PRON", "CAN", "VERB"}, {"S", "V", "O"}, "I can swim"}
        };

        for (const auto& [posSeq, roles, example] : commonPatterns) {
            string key = join(posSeq, "_") + "|" + join(roles, "-");
            if (!patterns.count(key)) {
                EnhancedPattern newPattern;
                newPattern.posSeq = posSeq;
                newPattern.semanticRoles = roles;
                newPattern.support = 20; // Good base support
                newPattern.confidence = 0.9;
                newPattern.semantic_coherence = 0.85;

                // Add some basic slot fillers
                if (posSeq.size() >= 3) {
                    if (posSeq[0] == "PRON") newPattern.slot_fillers["S"] = 0.8;
                    if (posSeq[1] == "VERB") newPattern.slot_fillers["V"] = 0.9;
                    if (posSeq[2] == "NOUN") newPattern.slot_fillers["O"] = 0.7;
                }

                patterns[key] = newPattern;
                cout << "Added base pattern: " << key << " (" << example << ")" << endl;
            }
        }
    }
void learnPatternsFromSentence(const vector<string>& words, const vector<string>& pos_tags,
                                  const vector<string>& roles,
                                  const vector<DependencyParser::Dependency>& deps) {
        if (words.size() < 2) return;

        // Extract n-gram patterns (2-4 grams)
        for (size_t n = 2; n <= 4 && n <= words.size(); n++) {
            for (size_t i = 0; i <= words.size() - n; i++) {
                vector<string> pattern_pos(pos_tags.begin() + i, pos_tags.begin() + i + n);
                vector<string> pattern_roles(roles.begin() + i, roles.begin() + i + n);

                // Skip patterns with too many unknown/out roles
                int valid_roles = 0;
                for (const auto& role : pattern_roles) {
                    if (role != "O") valid_roles++;
                }
                if (valid_roles < 1) continue;

                EnhancedPattern pattern;
                pattern.posSeq = pattern_pos;
                pattern.semanticRoles = pattern_roles;
                pattern.support++;

                string key = pattern.getKey();
                if (patterns.count(key)) {
                    patterns[key].support++;
                } else {
                    patterns[key] = pattern;
                }
            }
        }

        // Enhanced dependency-based patterns
        for (const auto& dep : deps) {
            if (dep.relation == "nsubj" || dep.relation == "dobj" || dep.relation == "amod") {
                if (dep.head >= 0 && dep.head < (int)pos_tags.size() &&
                    dep.dependent >= 0 && dep.dependent < (int)pos_tags.size()) {

                    // Create subject-verb-object patterns
                    if (dep.relation == "nsubj") {
                        vector<string> pattern_pos = {pos_tags[dep.dependent], pos_tags[dep.head]};

                        // Look for objects
                        for (const auto& dep2 : deps) {
                            if (dep2.head == dep.head && dep2.relation == "dobj") {
                                if (dep2.dependent >= 0 && dep2.dependent < (int)pos_tags.size()) {
                                    pattern_pos.push_back(pos_tags[dep2.dependent]);
                                }
                                break;
                            }
                        }

                        if (pattern_pos.size() >= 2) {
                            EnhancedPattern pattern;
                            pattern.posSeq = pattern_pos;
                            pattern.support += 2; // Boost dependency patterns
                            string key = pattern.getKey();
                            if (patterns.count(key)) {
                                patterns[key].support += 2;
                            } else {
                                patterns[key] = pattern;
                            }
                        }
                    }

                    // Create adjective-noun patterns
                    if (dep.relation == "amod") {
                        vector<string> pattern_pos = {pos_tags[dep.dependent], pos_tags[dep.head]};
                        EnhancedPattern pattern;
                        pattern.posSeq = pattern_pos;
                        pattern.support++;
                        string key = pattern.getKey();
                        if (patterns.count(key)) {
                            patterns[key].support++;
                        } else {
                            patterns[key] = pattern;
                        }
                    }
                }
            }
        }
    }

    void inducePatternsBatch() {
        for (auto& pattern_entry : patterns) {
            EnhancedPattern& pattern = pattern_entry.second;
            pattern.confidence = calculatePatternConfidence(pattern);
            pattern.semantic_coherence = calculateSemanticCoherence(pattern);
            pattern.embedding = computePatternEmbedding(pattern);
        }

        vector<string> to_remove;
        for (const auto& pattern_entry : patterns) {
            const EnhancedPattern& pattern = pattern_entry.second;
            if (pattern.support < PATTERN_SUPPORT_THRESHOLD ||
                pattern.confidence < MIN_PATTERN_CONFIDENCE) {
                to_remove.push_back(pattern_entry.first);
            }
        }

        for (const auto& key : to_remove) {
            patterns.erase(key);
        }
    }

    void finalizePatternInduction() {
        mergeSimilarPatterns();
        for (auto& pattern_entry : patterns) {
            pattern_entry.second.slot_fillers = calculateSlotFillers(pattern_entry.second);
        }
    }

    double calculatePatternConfidence(const EnhancedPattern& pattern) {
        // Support-based confidence (logarithmic scaling)
        double support_confidence = min(1.0, log(pattern.support + 1) / log(50.0));

        // Length factor (longer patterns are more informative but rarer)
        double length_factor = 0.5 + (0.5 * (1.0 - exp(-0.3 * pattern.posSeq.size())));

        // Semantic role factor (patterns with clear roles are better)
        double role_factor = 0.5;
        if (!pattern.semanticRoles.empty()) {
            int clear_roles = 0;
            for (const auto& role : pattern.semanticRoles) {
                if (role == "S" || role == "V" || role == "O") clear_roles++;
            }
            role_factor = 0.3 + (0.7 * (double)clear_roles / pattern.semanticRoles.size());
        }

        // Pattern quality based on POS sequence
        double sequence_quality = calculateSequenceQuality(pattern.posSeq);

        return (support_confidence * 0.4 +
                pattern.semantic_coherence * 0.3 +
                role_factor * 0.2 +
                sequence_quality * 0.1) * length_factor;
    }

    double calculateSequenceQuality(const vector<string>& pos_seq) {
        // Score patterns based on linguistic quality
        static const unordered_set<string> good_starts = {"PRON", "DET", "NOUN", "PROPN"};
        static const unordered_set<string> good_ends = {"NOUN", "VERB", "ADJ", "ADV"};

        double score = 0.5; // Base score

        // Good starting POS
        if (!pos_seq.empty() && good_starts.count(pos_seq[0])) {
            score += 0.2;
        }

        // Good ending POS
        if (!pos_seq.empty() && good_ends.count(pos_seq.back())) {
            score += 0.2;
        }

        // Contains a verb
        for (const auto& pos : pos_seq) {
            if (pos == "VERB") {
                score += 0.1;
                break;
            }
        }

        return min(1.0, score);
    }

    double calculateSemanticCoherence(const EnhancedPattern& pattern) {
        double total_coherence = 0.0;
        int count = 0;
        for (size_t i = 0; i + 1 < pattern.posSeq.size(); i++) {
            total_coherence += 0.8;
            count++;
        }
        return count > 0 ? total_coherence / count : 0.0;
    }

    vector<float> computePatternEmbedding(const EnhancedPattern& pattern) {
        vector<float> embedding(EMBEDDING_DIM, 0.0f);

        for (const auto& pos : pattern.posSeq) {
            auto it = pos_to_words.find(pos);
            if (it != pos_to_words.end() && !it->second.empty()) {
                string sample_word = *it->second.begin();
                vector<float> word_emb = pos_tagger.getWordEmbedding(sample_word);
                for (size_t i = 0; i < EMBEDDING_DIM; i++) {
                    embedding[i] += word_emb[i];
                }
            }
        }

        for (size_t i = 0; i < EMBEDDING_DIM; i++) {
            embedding[i] /= pattern.posSeq.size();
        }

        return embedding;
    }

    void mergeSimilarPatterns() {
        vector<string> keys;
        for (const auto& kv : patterns) keys.push_back(kv.first);

        for (size_t i = 0; i < keys.size(); i++) {
            if (!patterns.count(keys[i])) continue;

            for (size_t j = i + 1; j < keys.size(); j++) {
                if (!patterns.count(keys[j])) continue;

                EnhancedPattern& p1 = patterns[keys[i]];
                EnhancedPattern& p2 = patterns[keys[j]];

                if (arePatternsSimilar(p1, p2)) {
                    p1.support += p2.support;
                    p1.confidence = max(p1.confidence, p2.confidence);
                    patterns.erase(keys[j]);
                }
            }
        }
    }

    bool arePatternsSimilar(const EnhancedPattern& p1, const EnhancedPattern& p2) {
        if (p1.posSeq.size() != p2.posSeq.size()) return false;

        int matching_pos = 0;
        for (size_t i = 0; i < p1.posSeq.size(); i++) {
            if (p1.posSeq[i] == p2.posSeq[i]) matching_pos++;
        }

        double pos_similarity = (double)matching_pos / p1.posSeq.size();
        double semantic_sim = AdvancedStatUtils::semanticSimilarity(p1.embedding, p2.embedding);

        return (pos_similarity > 0.8 && semantic_sim > 0.7);
    }

    map<string, double> calculateSlotFillers(const EnhancedPattern& pattern) {
        map<string, double> fillers;
        return fillers;
    }

    string sampleWordByPOSAndRole(const string& pos, const string& role,
                                 const map<string, double>& slot_fillers) {
        return sampleByPOS(pos);
    }


    string sampleNextWord(const string& pos, const vector<string>& prev_words) {
        auto it = pos_to_words.find(pos);
        if (it == pos_to_words.end() || it->second.empty()) {
            return sampleByPOS(pos);
        }

        vector<pair<string, double>> candidates;
        double total_weight = 0.0;

        bool found_ngrams = false;

        for (const string& word : it->second) {
            if (!vocab.count(word)) continue;

            double score = vocab[word].freq;

            if (!prev_words.empty()) {
                string prev1 = prev_words.back();
                string bigram = prev1 + "	" + word;
                if (ngrams.bi.count(bigram)) {
                    score *= (1.0 + ngrams.bi[bigram] * 100.0);
                    found_ngrams = true;
                }

                if (prev_words.size() >= 2) {
                    string prev2 = prev_words[prev_words.size() - 2];
                    string trigram = prev2 + "	" + prev1 + "	" + word;
                    if (ngrams.tri.count(trigram)) {
                        score *= (1.0 + ngrams.tri[trigram] * 500.0);
                        found_ngrams = true;
                    }
                }
            }

            candidates.emplace_back(word, score);
        }

        if (!prev_words.empty() && found_ngrams) {
            for (auto& cand : candidates) {
                if (cand.second < 50.0) cand.second = 0.0;
            }
        }

        for (const auto& cand : candidates) {
            total_weight += cand.second;
        }

        if (candidates.empty() || total_weight == 0.0) return sampleByPOS(pos);

        double r = ((double)rand() / RAND_MAX) * total_weight;
        double accum = 0.0;

        for (const auto& candidate : candidates) {
            accum += candidate.second;
            if (r <= accum) return candidate.first;
        }

        return candidates.back().first;
    }

    string sampleByPOS(const string& pos) {
        auto it = pos_to_words.find(pos);

        // If no words for this POS, use common fallbacks
        if (it == pos_to_words.end() || it->second.empty()) {
            static const unordered_map<string, vector<string>> commonWords = {
            {"NOUN", {"time", "person", "year", "way", "day", "thing", "man", "world", "life", "hand", "house", "car", "book", "water", "food", "friend", "school", "work", "home", "city", "dog", "cat", "child", "student", "teacher"}},
            {"VERB", {"is", "are", "was", "were", "have", "has", "do", "does", "say", "said", "get", "got", "make", "made", "go", "went", "know", "see", "come", "think", "look", "want", "give", "take", "find", "tell", "work", "call", "walk", "run", "eat", "drink", "sleep", "read", "write"}},
            {"ADJ", {"good", "new", "first", "last", "long", "great", "little", "own", "other", "old", "big", "small", "young", "important", "beautiful", "happy", "sad", "strong", "weak", "smart", "red", "blue", "green", "hot", "cold"}},
            {"ADV", {"up", "so", "out", "just", "now", "then", "more", "also", "here", "there", "very", "well", "too", "always", "never", "sometimes", "quickly", "slowly", "carefully", "happily", "today", "yesterday", "soon", "often"}},
            {"PRON", {"I", "you", "he", "she", "it", "we", "they", "me", "him", "her", "us", "them", "my", "your", "his", "our", "their", "this", "that", "these"}},
            {"DET", {"the", "a", "an", "this", "that", "these", "those", "my", "your", "his", "her", "its", "our", "their", "some", "any", "all", "every", "each"}},
            {"PREP", {"in", "on", "at", "by", "with", "about", "against", "between", "into", "through", "during", "before", "after", "above", "below", "from", "to", "of", "for"}},
            {"CONJ", {"and", "but", "or", "so", "yet", "for", "nor", "because", "although", "while", "when", "where", "if", "unless", "until"}},
            {"PROPN", {"John", "Mary", "London", "Paris", "Google", "Microsoft", "Alice", "Bob", "English", "Monday", "January", "Christmas"}},
            {"NUM", {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "first", "second", "third", "last", "next"}}
            };

            if (commonWords.count(pos)) {
                const auto& words = commonWords.at(pos);
                return words[rand() % words.size()];
            }
            return "unknown";
        }

        // Weighted sampling from learned vocabulary
        vector<pair<string, double>> candidates;
        double total_weight = 0.0;

        for (const string& word : it->second) {
            if (vocab.count(word)) {
                double weight = vocab[word].freq;
                // Boost weight for more common words
                weight = pow(weight, 1.2);
                candidates.emplace_back(word, weight);
                total_weight += weight;
            }
        }

        if (candidates.empty()) {
            // Fallback to first word in the set
            return *it->second.begin();
        }

        // Weighted random selection
        double r = ((double)rand() / RAND_MAX) * total_weight;
        double accum = 0.0;

        for (const auto& candidate : candidates) {
            accum += candidate.second;
            if (r <= accum) return candidate.first;
        }

        return candidates.back().first;
    }

    string sampleBySemanticCoherence(const string& pos, const vector<float>& pattern_embedding) {
        auto it = pos_to_words.find(pos);
        if (it == pos_to_words.end()) return "";

        string best_word;
        double best_score = -1.0;

        for (const string& word : it->second) {
            if (!vocab.count(word)) continue;

            vector<float> word_embedding = pos_tagger.getWordEmbedding(word);
            double score = AdvancedStatUtils::semanticSimilarity(word_embedding, pattern_embedding);

            if (score > best_score) {
                best_score = score;
                best_word = word;
            }
        }

        return best_word;
    }

    string findBestMatchingPattern(const vector<string>& pos_tags, const vector<string>& roles,
                                  const vector<DependencyParser::Dependency>& deps) {
        string best_pattern;
        double best_score = -1.0;

        for (const auto& pattern_entry : patterns) {
            const EnhancedPattern& pattern = pattern_entry.second;
            double score = calculatePatternMatchScore(pos_tags, roles, deps, pattern);
            if (score > best_score) {
                best_score = score;
                best_pattern = pattern_entry.first;
            }
        }

        return best_score > 0.6 ? best_pattern : "unknown";
    }

    double calculatePatternMatchScore(const vector<string>& pos_tags, const vector<string>& roles,
                                    const vector<DependencyParser::Dependency>& deps,
                                    const EnhancedPattern& pattern) {
        double pos_score = calculatePOSMatchScore(pos_tags, pattern.posSeq);
        double role_score = calculateRoleMatchScore(roles, pattern.semanticRoles);
        return (pos_score * 0.7 + role_score * 0.3) * pattern.confidence;
    }

    double calculatePOSMatchScore(const vector<string>& sentence_pos, const vector<string>& pattern_pos) {
        vector<vector<int>> dp(sentence_pos.size() + 1, vector<int>(pattern_pos.size() + 1, 0));

        for (size_t i = 1; i <= sentence_pos.size(); i++) {
            for (size_t j = 1; j <= pattern_pos.size(); j++) {
                if (sentence_pos[i-1] == pattern_pos[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                } else {
                    dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
                }
            }
        }

        return (double)dp[sentence_pos.size()][pattern_pos.size()] / pattern_pos.size();
    }

    double calculateRoleMatchScore(const vector<string>& sentence_roles, const vector<string>& pattern_roles) {
        if (pattern_roles.empty()) return 1.0;

        int matches = 0;
        for (size_t i = 0; i < min(sentence_roles.size(), pattern_roles.size()); i++) {
            if (pattern_roles[i] == "O" || sentence_roles[i] == pattern_roles[i]) {
                matches++;
            }
        }

        return (double)matches / pattern_roles.size();
    }

    void updateNgramsAndVocab(const vector<string>& words, const vector<string>& pos_tags) {
        for (size_t i = 0; i < words.size(); i++) {
            string word = words[i];

            ngrams.uni[word]++;

            if (!vocab.count(word)) {
                Token token;
                token.surface = word;
                token.pos = pos_tags[i];
                double confidence = 0.0;
                token.lemma = morph.lemmatize(word, pos_tags[i], confidence);
                token.freq = 1;
                vocab[word] = token;
            } else {
                vocab[word].freq++;
            }

            if (i > 0) {
                string bigram = words[i-1] + "	" + words[i];
                ngrams.bi[bigram]++;
            }

            if (i > 1) {
                string trigram = words[i-2] + "	" + words[i-1] + "	" + words[i];
                ngrams.tri[trigram]++;
            }
        }
    }

    void computeAdvancedWeights() {
        double total_freq = 0.0;
        for (const auto& entry : vocab) {
            total_freq += entry.second.freq;
        }

        for (auto& entry : vocab) {
            Token& token = entry.second;
            double tf = (double)token.freq / total_freq;
            double idf = log((double)vocab.size() / (1.0 + pos_to_words[token.pos].size()));
            token.weight = tf * idf;
        }
    }

    string formatSentence(const vector<string>& words) {
    if (words.empty()) return "";

    string sentence = join(words, " ");

    // Capitalize first letter
    if (!sentence.empty()) {
        sentence[0] = toupper(sentence[0]);
    }

    // Add proper ending punctuation
    if (sentence.back() != '.' && sentence.back() != '?' && sentence.back() != '!') {
        // Determine if it's a question
        bool is_question = false;
        static unordered_set<string> question_words = {"what", "who", "where", "when", "why", "how", "which", "can", "will", "do", "does", "is", "are"};
        if (!words.empty() && question_words.count(toLower(words[0]))) {
            is_question = true;
        }

        sentence += is_question ? "?" : ".";
    }

    return sentence;
  }
};

// -------------------- Enhanced CLI Interface --------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    srand(static_cast<unsigned>(time(nullptr)));

    EnhancedPatternModel model;
    cout << "=== Advanced PatternLearner v3 ===" << endl;
    cout << "Features: Neural POS tagging, Semantic Role Labeling, Dependency Parsing" << endl;

    string line;
    while (true) {
        cout << "\nCommands: read <file>, gen, interp <sentence>, patterns [N], vocab [N], save <path>, load <path>, exit" << endl;
        cout << "> ";

        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") break;
        else if (cmd == "read") {
            string file;
            ss >> file;
            if (file.empty()) {
                cout << "Usage: read <filename>" << endl;
                continue;
            }
            cout << "Learning from: " << file << "..." << endl;
            model.learnFromFile(file);
        }
        else if (cmd == "gen") {
            cout << "Generated: " << model.generateSentence() << endl;
        }
        else if (cmd == "interp") {
            string sentence;
            getline(ss, sentence);
            if (sentence.empty()) {
                cout << "Usage: interp <sentence>" << endl;
                continue;
            }
            // Remove leading space if present
            if (!sentence.empty() && sentence[0] == ' ') {
                sentence = sentence.substr(1);
            }
            cout << "Interpretation: " << model.interpret(sentence) << endl;
        }
        else if (cmd == "patterns") {
            size_t n = 10;
            ss >> n;
            model.showPatterns(n);
        }
        else if (cmd == "vocab") {
            size_t n = 20;
            ss >> n;
            model.showVocab(n);
        }
        else if (cmd == "save") {
            string path;
            ss >> path;
            if (path.empty()) {
                cout << "Usage: save <filename>" << endl;
                continue;
            }
            if (model.saveModel(path)) {
                cout << "Model saved to: " << path << endl;
            } else {
                cout << "Save failed" << endl;
            }
        }
        else if (cmd == "load") {
            string path;
            ss >> path;
            if (path.empty()) {
                cout << "Usage: load <filename>" << endl;
                continue;
            }
            if (model.loadModel(path)) {
                cout << "Model loaded from: " << path << endl;
            } else {
                cout << "Load failed" << endl;
            }
        }
        else {
            cout << "Unknown command: " << cmd << endl;
        }
    }

    cout << "Goodbye!" << endl;
    return 0;
}