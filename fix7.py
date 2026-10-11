import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Make the semantic sense check more robust
old_sense = """       bool makesSemanticSense(const vector<string>& words, const vector<string>& pos_tags) {
    if (words.size() < 2) return false;

    // Don't end with prepositions or determiners
    if (pos_tags.back() == "PREP" || pos_tags.back() == "DET") return false;

    // Check for basic subject-verb agreement issues
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (pos_tags[i] == "PRON" && pos_tags[i+1] == "VERB") {
            string subject = words[i];
            string verb = words[i+1];

            // Basic agreement checks
            if ((subject == "I" && verb == "are") ||
                (subject == "he" && verb == "are") ||
                (subject == "she" && verb == "are") ||
                (subject == "I" && verb == "is") ||
                (subject == "you" && verb == "is") ||
                (subject == "we" && verb == "is") ||
                (subject == "they" && verb == "is") ||
                (subject == "you" && verb == "am") ||
                (subject == "he" && verb == "am") ||
                (subject == "she" && verb == "am") ||
                (subject == "it" && verb == "am") ||
                (subject == "we" && verb == "am") ||
                (subject == "they" && verb == "am")) {
                return false;
            }
        }
        if (pos_tags[i] == "DET" && pos_tags[i+1] == "NOUN") {
            string det = toLower(words[i]);
            string noun = toLower(words[i+1]);
            bool detPlural = (det == "these" || det == "those" || det == "some" || det == "all" || det == "many");
            bool detSingular = (det == "a" || det == "an" || det == "this" || det == "that" || det == "every" || det == "each");
            bool nounPlural = (noun.length() > 1 && noun.back() == 's' && noun != "glass" && noun != "class" && noun != "boss" && noun != "gas" && noun != "bus");
            if (detPlural && !nounPlural) return false;
            if (detSingular && nounPlural) return false;
        }
    }

    return true;
  }"""

new_sense = """       bool makesSemanticSense(const vector<string>& words, const vector<string>& pos_tags) {
    if (words.size() < 2) return false;

    // Don't end with prepositions or determiners
    if (pos_tags.back() == "PREP" || pos_tags.back() == "DET" || pos_tags.back() == "CAN") return false;

    // Subject pronoun checks
    if (pos_tags[0] == "PRON") {
        string w = toLower(words[0]);
        if (w == "me" || w == "him" || w == "her" || w == "us" || w == "them") return false;
        if (w == "my" || w == "your" || w == "his" || w == "our" || w == "their") return false;
    }

    // Check for basic subject-verb agreement issues
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (pos_tags[i] == "PRON" && pos_tags[i+1] == "VERB") {
            string subject = toLower(words[i]);
            string verb = toLower(words[i+1]);

            // Basic agreement checks
            if ((subject == "i" && verb == "are") ||
                (subject == "he" && verb == "are") ||
                (subject == "she" && verb == "are") ||
                (subject == "it" && verb == "are") ||
                (subject == "i" && verb == "is") ||
                (subject == "you" && verb == "is") ||
                (subject == "we" && verb == "is") ||
                (subject == "they" && verb == "is") ||
                (subject == "you" && verb == "am") ||
                (subject == "he" && verb == "am") ||
                (subject == "she" && verb == "am") ||
                (subject == "it" && verb == "am") ||
                (subject == "we" && verb == "am") ||
                (subject == "they" && verb == "am") ||
                (subject == "he" && verb == "do") ||
                (subject == "she" && verb == "do") ||
                (subject == "it" && verb == "do") ||
                (subject == "i" && verb == "does") ||
                (subject == "you" && verb == "does") ||
                (subject == "we" && verb == "does") ||
                (subject == "they" && verb == "does") ||
                (subject == "he" && verb == "have") ||
                (subject == "she" && verb == "have") ||
                (subject == "it" && verb == "have") ||
                (subject == "i" && verb == "has") ||
                (subject == "you" && verb == "has") ||
                (subject == "we" && verb == "has") ||
                (subject == "they" && verb == "has")) {
                return false;
            }
        }
        if (pos_tags[i] == "DET" && pos_tags[i+1] == "NOUN") {
            string det = toLower(words[i]);
            string noun = toLower(words[i+1]);
            bool detPlural = (det == "these" || det == "those" || det == "some" || det == "all" || det == "many");
            bool detSingular = (det == "a" || det == "an" || det == "this" || det == "that" || det == "every" || det == "each");
            bool nounPlural = (noun.length() > 1 && noun.back() == 's' && noun != "glass" && noun != "class" && noun != "boss" && noun != "gas" && noun != "bus");
            if (detPlural && !nounPlural) return false;
            if (detSingular && nounPlural) return false;
        }
    }

    return true;
  }"""
code = code.replace(old_sense, new_sense)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
