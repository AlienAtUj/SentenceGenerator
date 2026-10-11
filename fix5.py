import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# I want to add some logic for DET + NOUN + VERB agreement
# "These life sleeps" is wrong because "These" is plural but "life" is singular and "sleeps" is singular.
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
    }

    return true;
  }"""

new_sense = """       bool makesSemanticSense(const vector<string>& words, const vector<string>& pos_tags) {
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
code = code.replace(old_sense, new_sense)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
