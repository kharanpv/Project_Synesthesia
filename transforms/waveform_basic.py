DEFAULT_BG = (0, 0, 0)
DEFAULT_WAVE = (255, 255, 255)
DEFAULT_THICKNESS = 2


def _color(params, key, default):
    value = params.get(key, default) if isinstance(params, dict) else default
    if (
        not isinstance(value, (tuple, list))
        or len(value) != 3
        or not all(isinstance(c, int) and 0 <= c <= 255 for c in value)
    ):
        raise ValueError(f"params['{key}'] must be an RGB triple of ints in 0..255")
    return tuple(value)


def _thickness(params):
    default = DEFAULT_THICKNESS
    value = params.get("thickness", default) if isinstance(params, dict) else default
    if not isinstance(value, int) or isinstance(value, bool) or value < 1:
        raise ValueError(
            "params['thickness'] must be an int >= 1 (stroke width in pixels)"
        )
    return value


def waveform_basic(clip, t, dt, canvas, params):
    if params is not None and not isinstance(params, dict):
        raise ValueError("params must be a dict of waveform_basic parameters")
    bg = _color(params, "bg", DEFAULT_BG)
    wave = _color(params, "wave", DEFAULT_WAVE)
    thickness = _thickness(params)

    samples, sr = clip
    canvas.clear(bg)
    a = int(t * sr)
    b = min(int((t + dt) * sr), samples.shape[-1])
    if b - a < 2:
        return
    stretch = samples[:, a:b] if samples.ndim == 2 else samples[a:b]
    mono = stretch.mean(axis=0) if stretch.ndim == 2 else stretch
    n = mono.shape[0]
    w, h = canvas.width, canvas.height
    xs = [j * (w - 1) / (n - 1) for j in range(n)]
    ys = [h / 2 * (1 - a_j) for a_j in mono]
    canvas.line(list(zip(xs, ys)), wave, width=thickness)
