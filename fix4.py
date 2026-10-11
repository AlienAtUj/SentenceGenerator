import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Fix 'wents' bug
old_conj = """    // Don't add 's' to already conjugated irregular verbs like 'is', 'has', 'was', 'does', 'did', etc
    static unordered_set<string> alreadyConjugated = {"is", "has", "was", "does", "did", "am", "are", "were", "had", "can", "will", "would", "should", "could"};"""
new_conj = """    // Don't add 's' to already conjugated irregular verbs like 'is', 'has', 'was', 'does', 'did', etc
    static unordered_set<string> alreadyConjugated = {"is", "has", "was", "does", "did", "am", "are", "were", "had", "can", "will", "would", "should", "could", "went", "made", "took", "came", "saw", "got", "knew", "thought", "found", "said"};"""
code = code.replace(old_conj, new_conj)

with open("AIVocab.cpp", "w") as f:
    f.write(code)
