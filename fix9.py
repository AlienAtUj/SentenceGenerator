import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix 'We can got' -> 'got' is past tense, 'can get' is correct
# Fix 'These is red' -> 'These' is plural

old_sense = """    // Check for bad modal sequences
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (pos_tags[i] == "CAN") {
            string v = toLower(words[i+1]);
            if (v == "are" || v == "is" || v == "am" || v == "were" || v == "was" || v == "does" || v == "did" || v == "has" || v == "had") {
                return false;
            }
        }
    }"""
new_sense = """    // Check for bad modal sequences
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (pos_tags[i] == "CAN") {
            string v = toLower(words[i+1]);
            if (v == "are" || v == "is" || v == "am" || v == "were" || v == "was" || v == "does" || v == "did" || v == "has" || v == "had" || v == "went" || v == "got" || v == "said" || v == "made" || v == "came" || v == "saw" || v == "took" || v == "knew" || v == "thought" || v == "found") {
                return false;
            }
        }
    }"""
code = code.replace(old_sense, new_sense)

old_agreement = """                (subject == "we" && verb == "has") ||
                (subject == "they" && verb == "has")) {
                return false;
            }
        }"""
new_agreement = """                (subject == "we" && verb == "has") ||
                (subject == "they" && verb == "has") ||
                (subject == "these" && verb == "is") ||
                (subject == "those" && verb == "is") ||
                (subject == "these" && verb == "has") ||
                (subject == "those" && verb == "has") ||
                (subject == "these" && verb == "was") ||
                (subject == "those" && verb == "was") ||
                (subject == "this" && verb == "are") ||
                (subject == "that" && verb == "are") ||
                (subject == "this" && verb == "were") ||
                (subject == "that" && verb == "were")) {
                return false;
            }
        }"""
code = code.replace(old_agreement, new_agreement)


with open("AIVocab.cpp", "w") as f:
    f.write(code)
