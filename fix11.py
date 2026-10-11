import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# I want to update `generateSentence` so it loops 10 times
old_gen = """    // Try up to 3 times to generate a good sentence
    for (int attempt = 0; attempt < 3; attempt++) {"""
new_gen = """    // Try up to 10 times to generate a good sentence
    for (int attempt = 0; attempt < 10; attempt++) {"""
code = code.replace(old_gen, new_gen)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
