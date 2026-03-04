# Masters Research — Image Classification

Deep learning research workspace implementing two training pipelines — CNN-LSTM and transfer learning — for repeatable image classification experiments.

## Stack

![Python](https://img.shields.io/badge/Python_3-3776AB?style=flat&logo=python&logoColor=white)
![TensorFlow](https://img.shields.io/badge/TensorFlow-FF6F00?style=flat&logo=tensorflow&logoColor=white)
![Keras](https://img.shields.io/badge/Keras-D00000?style=flat&logo=keras&logoColor=white)

---

## What This Project Does

- Trains image classifiers from class-subfolder datasets using two independent pipelines.
- Splits dataset into train/validation sets (default validation split: 0.2).
- Applies early stopping (`monitor=val_loss`, patience = 5) to prevent overfitting.
- Saves per-run artifacts: trained model, training curves, and confusion matrix.

## Pipelines

| Pipeline | Entry | Model |
|---|---|---|
| `cnn-lstm` | `experiments/cnn_lstm_experiment.py` | CNN feature extractor + LSTM temporal layer |
| `transfer-learning` | `experiments/transfer_learning_experiment.py` | Pretrained backbone with fine-tuning head |

## Directory Structure

```text
masters-research/
├── main.py                           ← CLI dispatcher
├── requirements.txt
├── experiments/
│   ├── cnn_lstm_experiment.py
│   ├── transfer_learning_experiment.py
│   └── legacy/
└── src/
    ├── data/data_loader.py           ← dataset loading and splitting
    ├── models/
    │   ├── cnn_lstm.py
    │   └── transfer_learning_model.py
    ├── training/
    │   ├── cnn_lstm_trainer.py
    │   └── transfer_learning_trainer.py
    └── utils/metrics.py
```

## Environment Setup

```bash
cd masters-research
python3 -m venv .venv
source .venv/bin/activate
pip install -U pip
pip install -r requirements.txt
```

## Dataset Format

`--data-dir` must follow class-subfolder layout:

```text
data/<dataset_name>/
├── class_a/
│   └── img001.jpg
└── class_b/
    └── img101.jpg
```

## Training Commands

### CNN-LSTM

```bash
python3 main.py cnn-lstm \
  --data-dir data/<dataset_name> \
  --output-dir artifacts/cnn_lstm \
  --epochs 30 \
  --batch-size 8 \
  --image-size 224 \
  --time-steps 1 \
  --learning-rate 1e-4
```

### Transfer Learning

```bash
python3 main.py transfer-learning \
  --data-dir data/<dataset_name> \
  --output-dir artifacts/transfer_learning \
  --epochs 30 \
  --batch-size 8 \
  --image-size 224 \
  --learning-rate 1e-4
```

## Outputs

Each run writes the following to `--output-dir`:

| File | Description |
|---|---|
| `*.keras` | Trained model |
| `*_history.png` | Train/validation accuracy and loss curves |
| `*_confusion_matrix.png` | Confusion matrix image |
| `*_confusion_matrix.xlsx` | Confusion matrix table |

## Notes

- `data/`, `artifacts/`, and `experiments/legacy/` are excluded from the public repository.
