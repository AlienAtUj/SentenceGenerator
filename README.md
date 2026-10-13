# Advanced PatternLearner (AIVocab)

AIVocab is an advanced C++ AI text generation application that leverages modern linguistics and **Quantum Pattern Learning (QPL)** theory with **Logical Circle Integration**. It uses neural morphology, Transformer-based Part-Of-Speech (POS) tagging approximations, Dependency Parsing, and Semantic Role Labeling to learn grammar patterns organically and generate highly coherent sentences from minimal input text.

## Features

- **Neural POS Tagging Approximation:** Extracts part-of-speech context dynamically based on surrounding n-grams and vocabulary embeddings.
- **Quantum Pattern Expansion:** Recognizes foundational sentence origin structures (e.g., `PRON_VERB_NOUN` -> `Subject-Verb-Object`) and uses logical circles to return patterns to their functional origins, efficiently learning without massive data dependency.
- **Neural Morphology:** Includes comprehensive irregular verb support, rule-based lemma processing, and strict subject-verb agreement logic.
- **Advanced Heuristics:** Safely drops malformed patterns (like trailing prepositions or dangling dependent possessives) to output robust, natural-sounding English.

## Setup and Compilation

Ensure you have a modern C++ compiler that supports C++11 or higher.

```bash
g++ AIVocab.cpp -o AIVocab
```

## Running the Application

Execute the compiled application directly:

```bash
./AIVocab
```

This will launch an interactive CLI.

### Commands:

- `read <file>`: Reads a text file (like `basic.txt`) and organically learns patterns, morphology, and POS sequences.
- `batch <number> <filename.csv>`: Automatically streams `<number>` sentences to a CSV file.
- `seed <N>`: Sets the random seed for reproducible outputs.
- `api`: Toggles API mode to suppress verbose logging.
- `gen`: Generates a coherent sentence based on the learned patterns and vocabulary.
- `interp <sentence>`: Interprets a user-provided sentence and prints the identified grammar pattern mapping.
- `patterns [N]`: Prints the top `N` highest-confidence grammatical patterns extracted.
- `vocab [N]`: Prints the top `N` most frequent vocabulary words learned.
- `save <path>`: Serializes the current learned patterns and vocabulary to a binary file.
- `load <path>`: Loads a previously saved binary model.
- `exit`: Exits the application.

## Theory

AIVocab is grounded in **Quantum Pattern Learning with Logical Circle Theory**. Instead of massive memorization protocols reliant on brute force (e.g. millions of text pairs), this AI extracts core structural mappings (Algebraic, Relational, Transformation patterns). By defining foundational circle pathways (like `I love pizza -> Subject-Verb-Object`), the model can complete the structural circle and substitute out parts of speech infinitely with extremely high efficiency data improvements (upwards of a 1000x data reduction).

### Edge and IoT Mode
If compiling for low-memory microcontrollers or running as a silent text engine, you can toggle API Mode:
- `api`: Suppresses all verbose log output (e.g. `Learning complete...`, `Generated:` tags) allowing standard out to pipe directly to C/Python bridges cleanly.

### Synthetic Data Generation
For machine learning pipelines, AIVocab provides deterministic mass generation functionality.
- `seed <N>`: Forces the random engine to use `<N>` to provide perfectly reproducible datasets.
- `batch <number> <filename.csv>`: Automatically streams `<number>` organic sentences to a structured CSV file without overloading memory.
