import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# "It were" -> fix agreement
old_sense = """                (subject == "they" && verb == "am") ||
                (subject == "he" && verb == "do") ||
                (subject == "she" && verb == "do") ||"""
new_sense = """                (subject == "they" && verb == "am") ||
                (subject == "it" && verb == "were") ||
                (subject == "he" && verb == "were") ||
                (subject == "she" && verb == "were") ||
                (subject == "i" && verb == "were") ||
                (subject == "he" && verb == "do") ||
                (subject == "she" && verb == "do") ||"""
code = code.replace(old_sense, new_sense)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
