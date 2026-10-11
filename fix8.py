import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix 'These can are' to block 'can are'
old_sense = """    // Don't end with prepositions or determiners
    if (pos_tags.back() == "PREP" || pos_tags.back() == "DET" || pos_tags.back() == "CAN") return false;"""
new_sense = """    // Don't end with prepositions or determiners
    if (pos_tags.back() == "PREP" || pos_tags.back() == "DET" || pos_tags.back() == "CAN") return false;

    // Check for bad modal sequences
    for (size_t i = 0; i < words.size() - 1; i++) {
        if (pos_tags[i] == "CAN") {
            string v = toLower(words[i+1]);
            if (v == "are" || v == "is" || v == "am" || v == "were" || v == "was" || v == "does" || v == "did" || v == "has" || v == "had") {
                return false;
            }
        }
    }"""
code = code.replace(old_sense, new_sense)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
