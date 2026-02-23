from src.training.transfer_learning_trainer import build_parser, run


if __name__ == "__main__":
    parser = build_parser()
    run(parser.parse_args())
