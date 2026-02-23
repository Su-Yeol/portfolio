# Masters Research

## Overview
This project organizes deep-learning experiments into modular training, model, and data components.
It is structured for reproducible experimentation and maintainable code evolution.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout
- Modular separation of model, training, and data logic

## Architecture
```text
masters-research
├── README.md
├── __pycache__
│   └── main.cpython-310.pyc
├── data
├── experiments
│   ├── __pycache__
│   ├── cnn_lstm_experiment.py
│   ├── legacy
│   └── transfer_learning_experiment.py
├── main.py
├── requirements.txt
└── src
    ├── __init__.py
    ├── __pycache__
    ├── data
    ├── models
    ├── training
    └── utils

11 directories, 7 files
```

## Tech Stack
- Language: Python
- Framework / Library: tensorflow>=2.12, keras>=2.12, numpy>=1.23, scikit-learn>=1.2, matplotlib>=3.7
- Build Tool: Project-specific scripts

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd masters-research
python3 -m pip install -r requirements.txt
```

### Run
```bash
cd masters-research
python3 main.py --help
```

## License
- Refer to the repository-level `LICENSE` and policy documents.

## Private Components
- `data/`: raw or licensed datasets (excluded from Git upload).
- `artifacts/`: model outputs and experiment artifacts (excluded from Git upload).
- `experiments/legacy/`: internal legacy experiment scripts (excluded from Git upload).
