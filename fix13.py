import re

with open("AIVocab.cpp", "r") as f:
    code = f.read()

# Make sure "I were" isn't strictly banned, "If I were you" is valid English, but for simple SV it's usually "I was".
# The above fix did that. I also want to make sure I completed all the items in the plan.
# I already fixed:
# - fallback preventing "unknown" (fix2.py)
# - Verb Conjugation logic (do->does, go->goes, irregular plural matches, wents/mades) (fix3.py, fix4.py, fix10.py)
# - semantic sense checks (CAN followed by base verb, object pronouns as subjects) (fix7.py, fix8.py, fix9.py, fix12.py)
# - Pattern selection randomly (fix.py) and looping 10 times (fix11.py).
