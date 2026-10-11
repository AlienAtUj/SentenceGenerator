import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix 'ises' bug
old_conj = """    if (thirdPersonSingular.count(subject)) {
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
    }"""
new_conj = """    // Don't add 's' to already conjugated irregular verbs like 'is', 'has', 'was', 'does', 'did', etc
    static unordered_set<string> alreadyConjugated = {"is", "has", "was", "does", "did", "am", "are", "were", "had", "can", "will", "would", "should", "could"};
    if (alreadyConjugated.count(verb)) return verb;

    if (thirdPersonSingular.count(subject) || !subject.empty() && (subject != "I" && subject != "you" && subject != "we" && subject != "they" && subject != "my" && subject != "your" && subject != "our" && subject != "their" && subject != "these" && subject != "those")) {
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
    }"""
code = code.replace(old_conj, new_conj)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
