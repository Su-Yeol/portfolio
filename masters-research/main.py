from __future__ import annotations

import argparse


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Deep learning model training entrypoint")
    subparsers = parser.add_subparsers(dest="command", required=True)

    cnn_parser = subparsers.add_parser("cnn-lstm", help="Train CNN-LSTM model")
    cnn_parser.add_argument("--data-dir", required=True, help="Directory with class subfolders")
    cnn_parser.add_argument("--output-dir", default="artifacts/cnn_lstm")
    cnn_parser.add_argument("--image-size", type=int, default=224)
    cnn_parser.add_argument("--batch-size", type=int, default=8)
    cnn_parser.add_argument("--epochs", type=int, default=30)
    cnn_parser.add_argument("--time-steps", type=int, default=1)
    cnn_parser.add_argument("--learning-rate", type=float, default=1e-4)

    tl_parser = subparsers.add_parser("transfer-learning", help="Train transfer-learning model")
    tl_parser.add_argument("--data-dir", required=True, help="Directory with class subfolders")
    tl_parser.add_argument("--output-dir", default="artifacts/transfer_learning")
    tl_parser.add_argument("--image-size", type=int, default=224)
    tl_parser.add_argument("--batch-size", type=int, default=8)
    tl_parser.add_argument("--epochs", type=int, default=30)
    tl_parser.add_argument("--learning-rate", type=float, default=1e-4)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    if args.command == "cnn-lstm":
        from pathlib import Path
        from src.training import cnn_lstm_trainer

        args.data_dir = Path(args.data_dir)
        args.output_dir = Path(args.output_dir)
        cnn_lstm_trainer.run(args)
        return 0
    if args.command == "transfer-learning":
        from pathlib import Path
        from src.training import transfer_learning_trainer

        args.data_dir = Path(args.data_dir)
        args.output_dir = Path(args.output_dir)
        transfer_learning_trainer.run(args)
        return 0

    parser.error("Unknown command")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
