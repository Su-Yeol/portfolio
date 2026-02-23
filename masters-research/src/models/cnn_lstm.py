from __future__ import annotations

import tensorflow as tf


def build_cnn_lstm_model(
    num_classes: int,
    image_size: tuple[int, int] = (224, 224),
    channels: int = 3,
    time_steps: int = 1,
    dropout_rate: float = 0.5,
) -> tf.keras.Model:
    backbone_input = tf.keras.Input(shape=(image_size[0], image_size[1], channels))
    backbone = tf.keras.applications.ResNet50(
        include_top=False,
        weights="imagenet",
        input_tensor=backbone_input,
    )
    backbone.trainable = True

    sequence_input = tf.keras.Input(shape=(time_steps, image_size[0], image_size[1], channels))
    x = tf.keras.layers.TimeDistributed(backbone)(sequence_input)
    x = tf.keras.layers.TimeDistributed(tf.keras.layers.GlobalAveragePooling2D())(x)
    x = tf.keras.layers.Bidirectional(tf.keras.layers.LSTM(128, return_sequences=False))(x)
    x = tf.keras.layers.Dropout(dropout_rate)(x)
    x = tf.keras.layers.Dense(128, activation="relu")(x)
    x = tf.keras.layers.Dropout(dropout_rate)(x)
    outputs = tf.keras.layers.Dense(num_classes, activation="softmax")(x)
    return tf.keras.Model(inputs=sequence_input, outputs=outputs, name="cnn_lstm_classifier")
