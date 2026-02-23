from __future__ import annotations

import argparse
from pathlib import Path
import tensorflow as tf

from src.models.transfer_learning_model import build_transfer_learning_model
from src.data.data_loader import build_image_datasets
from src.utils.metrics import save_confusion_matrix, save_history_plot


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Train transfer-learning model")
    parser.add_argument("--data-dir", type=Path, required=True, help="Directory with class subfolders")
    parser.add_argument("--output-dir", type=Path, default=Path("artifacts/transfer_learning"))
    parser.add_argument("--image-size", type=int, default=224)
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--epochs", type=int, default=30)
    parser.add_argument("--learning-rate", type=float, default=1e-4)
    return parser


def run(args: argparse.Namespace) -> None:
    image_size = (args.image_size, args.image_size)
    train_ds, val_ds, class_names = build_image_datasets(
        data_dir=args.data_dir,
        image_size=image_size,
        batch_size=args.batch_size,
    )

    model = build_transfer_learning_model(
        num_classes=len(class_names),
        image_size=image_size,
    )
    model.compile(
        optimizer=tf.keras.optimizers.Adam(args.learning_rate),
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )

    callbacks = [tf.keras.callbacks.EarlyStopping(monitor="val_loss", patience=5, restore_best_weights=True)]
    history = model.fit(train_ds, validation_data=val_ds, epochs=args.epochs, callbacks=callbacks, verbose=2)

    args.output_dir.mkdir(parents=True, exist_ok=True)
    model.save(args.output_dir / "transfer_learning_model.keras")
    save_history_plot(history, args.output_dir, "transfer_learning")
    save_confusion_matrix(model, val_ds, class_names, args.output_dir, "transfer_learning")
