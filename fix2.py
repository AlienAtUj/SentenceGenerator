import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix the unknown word bug in sampleByPOS
old_fallback = """            if (commonWords.count(pos)) {
                const auto& words = commonWords.at(pos);
                return words[rand() % words.size()];
            }
            return "unknown";"""

new_fallback = """            if (pos == "CAN") {
                return "can";
            }
            if (commonWords.count(pos)) {
                const auto& words = commonWords.at(pos);
                return words[rand() % words.size()];
            }
            return "unknown";"""

code = code.replace(old_fallback, new_fallback)

# Fix logic so verbs are conjugated correctly
# Her new time see -> Her new time sees
old_subject_agreement = """            // Basic agreement checks
            if ((subject == "I" && verb == "are") ||
                (subject == "he" && verb == "are") ||
                (subject == "she" && verb == "are")) {
                return false;
            }"""

new_subject_agreement = """            // Basic agreement checks
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
            }"""

code = code.replace(old_subject_agreement, new_subject_agreement)


# Subject tracking logic needs fixing for DET NOUN VERB
old_subject_tracking = """            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0)) {
                last_subject = word;
            }"""

new_subject_tracking = """            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0) || (pos == "NOUN" && i < 2)) {
                last_subject = word;
            }"""

code = code.replace(old_subject_tracking, new_subject_tracking)


with open("AIVocab.cpp", "w") as f:
    f.write(code)
