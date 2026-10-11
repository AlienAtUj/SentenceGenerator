import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix "dos" -> "does"
old_conj = """            if (verb == "have") return "has";
            if (endsWith(verb, "ch") || endsWith(verb, "sh") || endsWith(verb, "x") || endsWith(verb, "s") || endsWith(verb, "z")) {"""
new_conj = """            if (verb == "have") return "has";
            if (verb == "do") return "does";
            if (verb == "go") return "goes";
            if (endsWith(verb, "ch") || endsWith(verb, "sh") || endsWith(verb, "x") || endsWith(verb, "s") || endsWith(verb, "z")) {"""
code = code.replace(old_conj, new_conj)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
