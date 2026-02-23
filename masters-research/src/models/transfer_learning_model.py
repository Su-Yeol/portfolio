from __future__ import annotations

import tensorflow as tf


def build_transfer_learning_model(
    num_classes: int,
    image_size: tuple[int, int] = (224, 224),
    channels: int = 3,
    dropout_rate: float = 0.5,
) -> tf.keras.Model:
    model_input = tf.keras.Input(shape=(image_size[0], image_size[1], channels))
    backbone = tf.keras.applications.ResNet50(
        include_top=False,
        weights="imagenet",
        input_tensor=model_input,
    )
    backbone.trainable = True

    x = tf.keras.layers.Flatten()(backbone.output)
    x = tf.keras.layers.Dense(1024, activation="relu")(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(dropout_rate)(x)
    x = tf.keras.layers.Dense(512, activation="relu")(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(dropout_rate)(x)
    x = tf.keras.layers.Dense(256, activation="relu")(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dense(128, activation="relu")(x)
    x = tf.keras.layers.BatchNormalization()(x)
    outputs = tf.keras.layers.Dense(num_classes, activation="softmax")(x)

    return tf.keras.Model(inputs=model_input, outputs=outputs, name="transfer_learning_classifier")
