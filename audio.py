import librosa
import numpy as np


def load(path):
    try:
        samples, sr = librosa.load(path, sr=None, mono=False)
    except Exception as e:
        raise RuntimeError(f"could not load {path}: {e}") from e
    return samples, sr


def duration(samples, sr):
    return samples.shape[-1] / sr


def peak(samples):
    return float(np.max(np.abs(samples)))
