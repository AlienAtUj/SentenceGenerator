import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix 'can thinks' -> 'can think' by checking previous word
old_gen = """            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0) || (pos == "NOUN" && i < 2)) {
                last_subject = word;
            }"""
new_gen = """            // Remember subject for verb conjugation
            if (semantic_role == "S" || (pos == "PRON" && i == 0) || (pos == "NOUN" && i < 2)) {
                last_subject = word;
            }
            // Reset subject if modal verb is used
            if (pos == "CAN") {
                last_subject = "";
            }"""

code = code.replace(old_gen, new_gen)


with open("AIVocab.cpp", "w") as f:
    f.write(code)
